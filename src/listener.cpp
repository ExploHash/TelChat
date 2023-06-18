#include "../include/listener.hpp"
#include <iostream>

void Listener::run() {
  // Initialize connection manager
  connection_manager_thread = std::thread(&Listener::initialize_connection_manager, this);

  // Initialize connection
  initialize_connection();

  // Listen for connections
  listen_for_connections();

  // Close connection
  close(socket_fd);
}

void Listener::initialize_connection() {
  // Initialize socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    std::cout << "Error creating socket" << std::endl;
    exit(1);
  }

  int reuse = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
    std::cout << "Error setting socket options" << std::endl;
    exit(1);
  }

  // Bind socket to port
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(8080);
  server_address.sin_addr.s_addr = INADDR_ANY;

  if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    std::cout << "Error binding socket to port" << std::endl;
    exit(1);
  }
}

void Listener::listen_for_connections() {
  if (listen(socket_fd, 5) < 0) {
    std::cout << "Error listening for connections" << std::endl;
    exit(1);
  }
  // Listen for connections
  while (true) {
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    std::cout << "Waiting for connection" << std::endl;
    int client_socket_fd = accept(socket_fd, (struct sockaddr*)&client_address, &client_address_size);
    std::cout << "Connection accepted" << std::endl;

    if (client_socket_fd < 0) {
      std::cout << "Error accepting connection" << std::endl;
      exit(1);
    }

    // Handle new connection
    handle_new_connection(client_socket_fd);

  }
}

void Listener::initialize_connection_manager() {
  ConnectionManager connection_manager;
  connection_manager.messages = &connection_manager_messages;
  connection_manager.messages_mutex = &connection_manager_messages_mutex;
  connection_manager.run();
}

void Listener::handle_new_connection(int client_socket_fd) {
  // Create new connection
  connection_manager_message message;
  message.type = ConnectionManagerMessageType::NEW_CONNECTION;
  message.socket = client_socket_fd;
  // Send message to connection manager
  std::scoped_lock lock(connection_manager_messages_mutex);
  connection_manager_messages.push(message);
}
