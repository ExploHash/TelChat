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
#include <fcntl.h>
#include "connection.hpp"
#include "connection_manager.hpp"

class Listener {
public:
  void run();

private:
  int socket_fd;
  std::thread connection_manager_thread;
  std::queue <connection_manager_message> connection_manager_messages;
  std::mutex connection_manager_messages_mutex;

  void initialize_connection();
  void initialize_connection_manager();
  void listen_for_connections();
  void handle_new_connection(int client_socket_fd);
};

#endif /* LISTENER_HPP */
