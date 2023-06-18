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
  for (auto const & connection_ptr : connections) {
    // First get raw pointer
    ConnectionState * connection = connection_ptr.get();

    // Check for new messages
    std::scoped_lock lock(connection->messages_mutex);
    if (!connection->messages.empty()) {
      // Get message
      connection_message message = connection->messages.front();
      connection->messages.pop();
      // Handle message
      handle_connection_message(message);
    }
  }
    
}

void ConnectionManager::handle_connection_message(connection_message message) {
  std::cout << "Connection message" << std::endl;
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

void ConnectionManager::spawn_new_connection(int socket_fd, std::queue <connection_message> * messages, std::mutex * messages_mutex) {
   // Create connection
  Connection connection;
  connection.messages = messages;
  connection.messages_mutex = messages_mutex;
  // Log socket fd
  connection.socket_fd = socket_fd;
  // Run connection
  connection.run();
}