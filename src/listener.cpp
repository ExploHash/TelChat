#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

class Listener {
public:
  int socket_fd;
  void listen_for_connections() {
    // Initialize socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
      std::cout << "Error creating socket" << std::endl;
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

    // Listen for connections
    if (listen(socket_fd, 5) < 0) {
      std::cout << "Error listening for connections" << std::endl;
      exit(1);
    }

    // Accept connections
    while (true) {
      struct sockaddr_in client_address;
      socklen_t client_address_size = sizeof(client_address);
      int client_socket_fd = accept(socket_fd, (struct sockaddr*)&client_address, &client_address_size);

      if (client_socket_fd < 0) {
        std::cout << "Error accepting connection" << std::endl;
        exit(1);
      }

      // Write hello world to client
      char message[] = "Hello World\n";
      write(client_socket_fd, message, sizeof(message));
      close(client_socket_fd);
    }

    // Close connection
    close(socket_fd);
  }
};