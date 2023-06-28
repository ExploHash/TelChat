#include "../include/connection.hpp"
#include <iostream>

void Connection::run() {
  // Initialize render
  renderer.socket_fd = socket_fd;

  // Spawn thread to read input
  input_reader_thread = std::thread(&Connection::spawn_input_reader, this);

  // Main loop
  while (is_running) {
    // Get possible new messages
    check_connection_messages();

    // Get possible new input messages
    check_connection_input_messages();

    if(!is_running) break;

    // Possibly render
    render();

    // Sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void Connection::check_connection_messages() {
  std::scoped_lock<std::mutex> lock(*messages_to_connection_mutex);

  while (!messages_to_connection->empty()) {
    connection_message message = messages_to_connection->front();
    messages_to_connection->pop();

    handle_connection_message(message);
  }
}

void Connection::handle_connection_message(connection_message message) {

  switch (message.type)
  {
  case ConnectionMessageType::MESSAGE_RECEIVE:
    std::cout << "[" << username << "] Handling message: " << message.text << std::endl;
    // Push to conversation_messages
    add_conversation_message(message.text, username_connected_to);

    render_needed = true;
    chat_preserve_input = true;
    break;

  case ConnectionMessageType::CONVERSATION_ACCEPTED:
    status = ConnectionStatus::CHAT;
    username_connected_to = message.text;
    render_needed = true;
    chat_preserve_input = false;
    break;

  case ConnectionMessageType::SHUTDOWN:
    shutdown_prepare();
    break;

  default:
    break;
  }
}

void Connection::check_connection_input_messages() {
  std::scoped_lock<std::mutex> lock(messages_from_input_reader_mutex);

  while (!messages_from_input_reader.empty()) {
    std::cout << "Handling input message" << std::endl;
    InputReaderMessage input = messages_from_input_reader.front();
    messages_from_input_reader.pop();

    if (input.type == InputReaderMessageType::INPUT_RECEIVED) {
      handle_input(input.text);
    } else if (input.type == InputReaderMessageType::SHUTDOWN_SUCCESSFUL) {
      shutdown();
    }
  }
}

void Connection::send_message(std::string message) {
  std::scoped_lock<std::mutex> lock(*messages_to_manager_mutex);
  //Create connection_message struct
  connection_message connection_message;
  connection_message.type = ConnectionMessageType::MESSAGE_SEND;
  connection_message.text = message;

  //Push to queue
  messages_to_manager->push(connection_message);

  // Add to conversation_messages
  add_conversation_message(message, username);
}

void Connection::handle_input(std::string input) {
  switch (status)
  {
  case ConnectionStatus::CHOOSE_USERNAME:
    if (input != "") {
      username = input;
      status = ConnectionStatus::CHOOSE_PERSON;
      // Send username to server
      send_username(username);
    }
    render_needed = true;
    break;

  case ConnectionStatus::CHOOSE_PERSON:
    if (input != "") {
      // Create connection_message struct
      std::scoped_lock<std::mutex> lock(*messages_to_manager_mutex);
      connection_message connection_message;
      connection_message.type = ConnectionMessageType::CONVERSATION_WANTS;
      connection_message.text = input;

      // Push to queue
      messages_to_manager->push(connection_message);

      // Set status to SHOW_MESSAGE
      status = ConnectionStatus::SHOW_MESSAGE;
      std::cout << input << std::endl;
      message_to_show = "Waiting for " + std::string(input) + " to accept...";
      render_needed = true;
    }
    break;

  case ConnectionStatus::CHAT:
    if (input == ":quit") {
      initiated_shutdown = true;
      send_leave();
    }
    else if (input != "") {
      send_message(input);
    }

    render_needed = true;
    chat_preserve_input = false;
    break;

  default:
    break;
  }
}

void Connection::render() {
  if (render_needed) {
    switch (status)
    {
    case ConnectionStatus::CHOOSE_USERNAME:
      renderer.render_input("Choose username: ");
      break;

    case ConnectionStatus::CHOOSE_PERSON:
      renderer.render_input("Choose other username: ");
      break;

    case ConnectionStatus::SHOW_MESSAGE:
      std::cout << "Showing message" << std::endl;
      renderer.render_message(message_to_show);
      break;

    case ConnectionStatus::CHAT:
      renderer.render_chat(conversation_messages, chat_preserve_input);
      break;

    default:
      break;
    }
  }

  render_needed = false;
}

void Connection::send_username(std::string username) {
  std::scoped_lock<std::mutex> lock(*messages_to_manager_mutex);
  // Create connection_message struct
  connection_message connection_message;
  connection_message.type = ConnectionMessageType::USERNAME;
  connection_message.text = username;

  // Push to queue
  messages_to_manager->push(connection_message);
}


void Connection::add_conversation_message(std::string message, std::string sender_name) {
  // Only keep the last 7 messages
  if (conversation_messages.size() == 6) {
    conversation_messages.pop_front();
  }

  // Create conversation_message struct
  conversation_message conversation_message;
  conversation_message.text = message;
  conversation_message.sender_name = sender_name;

  // Push to list
  conversation_messages.push_back(conversation_message);
}

void Connection::spawn_input_reader() {
  // Create input_reader
  InputReader input_reader;
  input_reader.socket_fd = socket_fd;
  input_reader.messages_from_reader = &messages_from_input_reader;
  input_reader.messages_from_reader_mutex = &messages_from_input_reader_mutex;
  input_reader.messages_to_reader = &messages_to_input_reader;
  input_reader.messages_to_reader_mutex = &messages_to_input_reader_mutex;

  // Run input_reader
  input_reader.run();

  std::cout << "Input reader stopped" << std::endl;
}

void Connection::send_leave() {
  std::cout << "Leaving" << std::endl;
  std::scoped_lock<std::mutex> lock(*messages_to_manager_mutex);
  // Create connection_message struct
  connection_message connection_message;
  connection_message.type = ConnectionMessageType::WANTS_LEAVE;

  // Push to queue
  messages_to_manager->push(connection_message);
}

void Connection::shutdown_prepare() {
  std::cout << "Preparing shutting down" << std::endl;
  // First show message
  std::string leave_message;
  if (initiated_shutdown) {
    leave_message = "\nYou left the conversation\n";
  }
  else {
    leave_message = "\n" + username_connected_to + " left the conversation\n";
  }

  write(socket_fd, leave_message.c_str(), leave_message.size());

  // Stop the input reader main loop
  std::scoped_lock<std::mutex> lock_renderer(messages_to_input_reader_mutex);
  InputReaderMessage message;
  message.type = InputReaderMessageType::SHUTDOWN;
  messages_to_input_reader.push(message);

  std::cout << "Input reader send shutdown" << std::endl;
}

void Connection::shutdown() {
  std::cout << "Shutting down" << std::endl;
  // Close connection
  close(socket_fd);

  // Close thread
  input_reader_thread.join();

  // Stop the main loop
  is_running = false;

  // Send shutdown successfull
  std::scoped_lock<std::mutex> lock(*messages_to_manager_mutex);
  connection_message connection_message;
  connection_message.type = ConnectionMessageType::SHUTDOWN_SUCCESSFULL;

  // Push to queue
  messages_to_manager->push(connection_message);
}