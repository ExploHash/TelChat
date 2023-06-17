#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <list>
// #include "connection.hpp"

extern int socket_fd;

void signal_callback_handler(int signum);

class Listener {
public:
  std::list<std::thread> threads;

  void run();

private:
  void initialize_connection();
  void listen_for_connections();
  void handle_new_connection(int client_socket_fd);
};

#endif /* LISTENER_HPP */
