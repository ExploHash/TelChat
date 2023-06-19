#include "../include/connection_manager.hpp"

#include <iostream>

void ConnectionManager::run() {
  while (true) {
    // Check for new messages from listener
    check_for_manager_messages();
    // Check for new messages from connections
    check_for_connection_messages();
    // Sleep for 100ms
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
    // First get raw pointer
    ConnectionState* connection = connection_ptr.get();

    // Check for new messages
    connection->messages_mutex.lock();
    if (!connection->messages.empty()) {
      // Get message
      connection_message message = connection->messages.front();
      connection->messages.pop();
      // Delete lock
      connection->messages_mutex.unlock();
      // Handle message
      handle_connection_message(message, connection);
    }else{
      // Delete lock
      connection->messages_mutex.unlock();
    }
  }

}

void ConnectionManager::handle_connection_message(connection_message message, ConnectionState* current_connection_state) {
  switch (message.type)
  {
  case ConnectionMessageType::MESSAGE_SEND: {
    std::string other_username = current_connection_state->username_connected_to;
    // Find connectionstate by username
    ConnectionState* other_connection_state = find_connection_state_by_username(other_username);

    // Push message to other_connection_state
    std::scoped_lock lock(other_connection_state->messages_mutex);
    message.type = ConnectionMessageType::MESSAGE_RECEIVE;
    other_connection_state->messages.push(message);
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
    ConnectionState* other_connection_state = find_connection_state_by_username(message.text);

    // Check if other_connection_state is not nullptr
    if (other_connection_state == nullptr) {
      std::cout << "Did not find connectionstate" << std::endl;
    } else if (other_connection_state->username_connected_to == current_connection_state->username) {
      std::cout << "Found connectionstate AAAAA" << std::endl;
      // Set username_connected_to
      other_connection_state->username_connected_to = current_connection_state->username;
      other_connection_state->is_connected = true;
      current_connection_state->is_connected = true;

      // Send message to current_connection_state
      std::scoped_lock lock(current_connection_state->messages_mutex);
      message.type = ConnectionMessageType::CONVERSATION_ACCEPTED;
      current_connection_state->messages.push(message);

      // Send message to other_connection_state
      message.type = ConnectionMessageType::CONVERSATION_ACCEPTED;
      other_connection_state->messages.push(message);
    }

    break;
  }

  default:
    break;
  }
}

void ConnectionManager::handle_manager_message(connection_manager_message message) {
  std::cout << "Manager message" << std::endl;
  switch (message.type) {
  case ConnectionManagerMessageType::NEW_CONNECTION:
    // Create connectionstate
    auto connection_state(std::make_unique<ConnectionState>());

    // Create connection
    connection_state->connection_thread = std::thread(&ConnectionManager::spawn_new_connection, this, message.socket, &connection_state->messages, &connection_state->messages_mutex);

    // Add connectionstate to list
    connections.push_back(std::move(connection_state));

    break;
  }
}

void ConnectionManager::spawn_new_connection(int socket_fd, std::queue <connection_message>* messages, std::mutex* messages_mutex) {
  // Create connection
  Connection connection;
  connection.messages = messages;
  connection.messages_mutex = messages_mutex;
  // Log socket fd
  connection.socket_fd = socket_fd;
  // Run connection
  connection.run();
}

ConnectionState* ConnectionManager::find_connection_state_by_username(std::string username) {
  for (auto const& connection_ptr : connections) {
    // First get raw pointer
    ConnectionState* connection = connection_ptr.get();

    // Check if username matches
    std::cout << "Cur Username: " << connection->username << std::endl;
    std::cout << "wants Username: " << username << std::endl;
    if (connection->username == username) {
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