#include "../include/connection.hpp"
#include <iostream>

void Connection::run() {
  write(socket_fd, "Hello World!\n", 13);
}


void Connection::listen_for_input() {
  std::cout << "Listening for input" << std::endl;
}