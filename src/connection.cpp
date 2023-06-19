#include "../include/connection.hpp"
#include <iostream>

void Connection::run() {
  // Initialize render
  renderer.socket_fd = socket_fd;

  // Spawn thread to read input
  input_reader_thread = std::thread(&Connection::spawn_input_reader, this);

  // Main loop
  while (true) {
    // Get possible new messages
    check_connection_messages();

    // Get possible new input messages
    check_connection_input_messages();

    // Possibly render
    render();

    // Sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void Connection::check_connection_messages() {
  std::scoped_lock<std::mutex> lock(*messages_mutex);

  while (!messages->empty()) {
    std::cout << "Handling message" << std::endl;
    connection_message message = messages->front();
    messages->pop();

    handle_connection_message(message);
  }
}

void Connection::handle_connection_message(connection_message message) {

  switch (message.type)
  {
  case ConnectionMessageType::MESSAGE_RECEIVE:
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

  default:
    break;
  }
}

void Connection::check_connection_input_messages() {
  std::scoped_lock<std::mutex> lock(connection_input_messages_mutex);

  while (!connection_input_messages.empty()) {
    std::cout << "Handling input message" << std::endl;
    std::string input = connection_input_messages.front();
    connection_input_messages.pop();

    handle_input(input);
  }
}

void Connection::send_message(std::string message) {
  std::scoped_lock<std::mutex> lock(*messages_mutex);
  //Create connection_message struct
  connection_message connection_message;
  connection_message.type = ConnectionMessageType::MESSAGE_SEND;
  connection_message.text = message;

  //Push to queue
  messages->push(connection_message);

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
      std::scoped_lock<std::mutex> lock(*messages_mutex);
      connection_message connection_message;
      connection_message.type = ConnectionMessageType::CONVERSATION_WANTS;
      connection_message.text = input;

      // Push to queue
      messages->push(connection_message);

      // Set status to SHOW_MESSAGE
      status = ConnectionStatus::SHOW_MESSAGE;
      std::cout << input << std::endl;
      message_to_show = "Waiting for " + std::string(input) + " to accept...";
      render_needed = true;
    }
    break;

  case ConnectionStatus::CHAT:
    if (input != "") {
      send_message(input);
    }
    render_needed = true;
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
  std::scoped_lock<std::mutex> lock(*messages_mutex);
  // Create connection_message struct
  connection_message connection_message;
  connection_message.type = ConnectionMessageType::USERNAME;
  connection_message.text = username;

  // Push to queue
  messages->push(connection_message);
}


void Connection::add_conversation_message(std::string message, std::string sender_name) {
  // Only keep the last 7 messages
  if (conversation_messages.size() == 7) {
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
  input_reader.messages = &connection_input_messages;
  input_reader.messages_mutex = &connection_input_messages_mutex;

  // Run input_reader
  input_reader.run();
}