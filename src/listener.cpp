#include "../include/listener.hpp"
#include <iostream>

int socket_fd;

void signal_callback_handler(int signum) {
  // Close socket
  close(socket_fd);
  std::cout << "Terminating program" << std::endl;

  // Terminate program
  exit(signum);
}

void Listener::run() {
  signal(SIGINT, signal_callback_handler);

  // // Initialize connection
  // initialize_connection();

  // // Listen for connections
  // listen_for_connections();

  // // Close connection
  // close(socket_fd);
}

// void Listener::initialize_connection() {
//   // Initialize socket
//   socket_fd = socket(AF_INET, SOCK_STREAM, 0);
//   if (socket_fd < 0) {
//     std::cout << "Error creating socket" << std::endl;
//     exit(1);
//   }

//   // Bind socket to port
//   struct sockaddr_in server_address;
//   server_address.sin_family = AF_INET;
//   server_address.sin_port = htons(8080);
//   server_address.sin_addr.s_addr = INADDR_ANY;

//   if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
//     std::cout << "Error binding socket to port" << std::endl;
//     exit(1);
//   }
// }

// void Listener::listen_for_connections() {
//   if (listen(socket_fd, 5) < 0) {
//     std::cout << "Error listening for connections" << std::endl;
//     exit(1);
//   }
//   // Listen for connections
//   while (true) {
//     struct sockaddr_in client_address;
//     socklen_t client_address_size = sizeof(client_address);
//     std::cout << "Waiting for connection" << std::endl;
//     int client_socket_fd = accept(socket_fd, (struct sockaddr*)&client_address, &client_address_size);
//     std::cout << "Connection accepted" << std::endl;

//     if (client_socket_fd < 0) {
//       std::cout << "Error accepting connection" << std::endl;
//       exit(1);
//     }

//     // Handle new connection in new thread
//     std::thread t1(&Listener::handle_new_connection, this, client_socket_fd);
//     threads.push_back(std::move(t1));
//   }
// }

// void Listener::handle_new_connection(int client_socket_fd) {
//   // Create connection
//   Connection connection;
//   // Log socket fd
//   std::cout << "Socket fd list: " << client_socket_fd << std::endl;
//   connection.socket_fd = client_socket_fd;

//   // Run connection
//   connection.run();
// }
