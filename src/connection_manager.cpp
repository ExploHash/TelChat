#include "../include/connection_manager.hpp"

#include <iostream>

void ConnectionManager::run() {
  while (true) {
    // Check for new messages from listener
    check_for_manager_messages();
    // Check for new messages from connections
    check_for_connection_messages();
    // Sleep for 100ms
    std::cout << "Sleeping for 100ms" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void ConnectionManager::check_for_manager_messages() {
  // Check for new messages from listener
  std::scoped_lock lock(*messages_mutex);
  if (!messages->empty()) {
    // Get message
    connection_manager_message message = messages->front();

    messages->pop();
    // Handle message
    handle_manager_message(message);
  }
}

void ConnectionManager::check_for_connection_messages() {
  // Check for new messages from connections
  for (auto const& connection_ptr : connections) {
    std::cout << "Checking for messages from connection" << std::endl;
    // First get raw pointer
    ConnectionState* connection = connection_ptr.get();

    // Check for new messages
    std::scoped_lock lock(connection->messages_to_manager_mutex);
    if (!connection->messages_to_manager.empty()) {
      // Get message
      connection_message message = connection->messages_to_manager.front();

      connection->messages_to_manager.pop();
      // Delete lock
      connection->messages_to_manager_mutex.unlock();
      // Handle message
      bool continue_loop = handle_connection_message(message, connection);
      std::cout << "Continue loop: " << continue_loop << std::endl;
      if (!continue_loop) break;
    }
  }

}

bool ConnectionManager::handle_connection_message(connection_message message, ConnectionState* current_connection_state) {
  switch (message.type)
  {
  case ConnectionMessageType::MESSAGE_SEND: {
    std::string other_username = current_connection_state->username_connected_to;
    // Find connectionstate by username
    ConnectionState* other_connection_state = find_connection_state_by_username(other_username, true);

    // Push message to other_connection_state
    std::scoped_lock lock(other_connection_state->messages_to_connection_mutex);
    message.type = ConnectionMessageType::MESSAGE_RECEIVE;
    other_connection_state->messages_to_connection.push(message);

    std::cout << "Message send: " << message.text << " to " << other_username << std::endl;
    break;
  }
  case ConnectionMessageType::USERNAME: {
    // Set username
    current_connection_state->username = message.text;
    std::cout << "Setting username: " << message.text << std::endl;
    break;
  }
  case ConnectionMessageType::CONVERSATION_WANTS: {
    // Add to connected_to
    current_connection_state->username_connected_to = message.text;

    std::cout << "Conversation wants: " << message.text << std::endl;

    // Try to find connectionstate by username
    ConnectionState* other_connection_state = find_connection_state_by_username(message.text, false);

    // Check if other_connection_state is not nullptr
    if (other_connection_state == nullptr) {
      std::cout << "Did not find connectionstate" << std::endl;
    }
    else if (other_connection_state->username_connected_to == current_connection_state->username) {
      std::cout << "Found connectionstate AAAAA" << std::endl;
      // Set username_connected_to
      other_connection_state->username_connected_to = current_connection_state->username;
      other_connection_state->is_connected = true;
      current_connection_state->is_connected = true;

      // Send message to current_connection_state
      std::scoped_lock lock(current_connection_state->messages_to_connection_mutex);
      connection_message message_accepted_1;
      message_accepted_1.type = ConnectionMessageType::CONVERSATION_ACCEPTED;
      message_accepted_1.text = other_connection_state->username;
      current_connection_state->messages_to_connection.push(message_accepted_1);

      // Send message to other_connection_state
      std::scoped_lock lock2(other_connection_state->messages_to_connection_mutex);
      connection_message message_accepted_2;
      message_accepted_2.type = ConnectionMessageType::CONVERSATION_ACCEPTED;
      message_accepted_2.text = current_connection_state->username;
      other_connection_state->messages_to_connection.push(message_accepted_2);
    }

    break;
  }

  case ConnectionMessageType::WANTS_LEAVE: {
    std::cout << "Wants shutdown" << std::endl;
    // Find other connection
    ConnectionState* other_connection_state = find_connection_state_by_username(current_connection_state->username_connected_to, true);
    // Send shutdown to both connections

    // Send message to current_connection_state
    std::scoped_lock lock(current_connection_state->messages_to_connection_mutex);
    connection_message message_shutdown_1;
    message_shutdown_1.type = ConnectionMessageType::SHUTDOWN;
    current_connection_state->messages_to_connection.push(message_shutdown_1);

    current_connection_state->connection_thread.detach();

    // Send message to other_connection_state
    std::scoped_lock lock2(other_connection_state->messages_to_connection_mutex);
    connection_message message_shutdown_2;
    message_shutdown_2.type = ConnectionMessageType::SHUTDOWN;
    other_connection_state->messages_to_connection.push(message_shutdown_2);

    other_connection_state->connection_thread.detach();

    //Set both connections to disconnected
    current_connection_state->is_connected = false;
    other_connection_state->is_connected = false;
  }

  case ConnectionMessageType::SHUTDOWN_SUCCESSFULL: {
    // Make copy of username
    std::string username = current_connection_state->username;
    // Remove current_connection_state variable
    current_connection_state = nullptr;
    // Delete connectionstate
    delete_connection_state_by_username(username);
    
    return false;
  }

  default:
    break;
  }

  return true;
}

void ConnectionManager::handle_manager_message(connection_manager_message message) {
  std::cout << "Manager message" << std::endl;
  switch (message.type) {
  case ConnectionManagerMessageType::NEW_CONNECTION:
    // Create connectionstate
    auto connection_state_temp(std::make_unique<ConnectionState>());
    connections.push_back(std::move(connection_state_temp));

    // Get connectionstate pointer
    ConnectionState* connection = connections.back().get();

    // Create connection
    connection->connection_thread = std::thread(
      &ConnectionManager::spawn_new_connection,
      this,
      message.socket,
      &connection->messages_to_manager,
      &connection->messages_to_manager_mutex,
      &connection->messages_to_connection,
      &connection->messages_to_connection_mutex
    );
    break;
  }
}

void ConnectionManager::spawn_new_connection(int socket_fd, std::queue <connection_message>* messages_to_manager, std::mutex* messages_to_manager_mutex, std::queue <connection_message>* messages_to_connection, std::mutex* messages_to_connection_mutex) {
  // Create connection
  Connection connection;
  connection.messages_to_manager = messages_to_manager;
  connection.messages_to_manager_mutex = messages_to_manager_mutex;
  connection.messages_to_connection = messages_to_connection;
  connection.messages_to_connection_mutex = messages_to_connection_mutex;
  // Log socket fd
  connection.socket_fd = socket_fd;
  // Run connection
  connection.run();

  std::cout << "Connection ended" << std::endl;
}

ConnectionState* ConnectionManager::find_connection_state_by_username(std::string username, bool is_connected) {
  for (auto const& connection_ptr : connections) {
    // First get raw pointer
    ConnectionState* connection = connection_ptr.get();

    // Check if username matches
    std::cout << "Cur Username: " << connection->username << std::endl;
    std::cout << "wants Username: " << username << std::endl;
    if (connection->username == username && connection->is_connected == is_connected) {
      return connection;
    }
  }
  return nullptr;
}

ConnectionState* ConnectionManager::find_connection_state_by_username_connected_to(std::string username) {
  for (auto const& connection_ptr : connections) {
    // First get raw pointer
    ConnectionState* connection = connection_ptr.get();

    // Check if username matches
    if (connection->username_connected_to == username) {
      return connection;
    }
  }
  return nullptr;
}

void ConnectionManager::delete_connection_state_by_username(std::string username) {
  std::cout << "Delete connection state" << std::endl;
  std::cout << "Username: " << username << std::endl;
  auto iter = connections.begin();
  while (iter != connections.end())
  {
    if ((*iter)->username == username)
    {
      std::cout << "Found connectionstate" << std::endl;
      std::cout << "Username: " << (*iter)->username << std::endl;
      connections.erase(iter);
      std:: cout << "Deleted connectionstate" << std::endl;
      break;
    }
    else
    {
      ++iter;
    }
  }
}