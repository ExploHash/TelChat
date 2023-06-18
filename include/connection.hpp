#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unistd.h>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include "renderer.hpp"
#include "connection_state.hpp"

class Connection {
public:
  int socket_fd;
  std::string username;
  std::string input;

  std::queue <connection_message> * messages;
  std::mutex * messages_mutex;

  void run();
  void listen_for_input();
};

#endif /* CONNECTION_HPP */
