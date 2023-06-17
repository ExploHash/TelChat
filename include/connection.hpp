#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unistd.h>
#include <string>
#include <thread>
#include <mutex>
#include "conversation.hpp"
#include "renderer.hpp"

class Connection {
public:
  int socket_fd;
  std::string username;

  Conversation conversation;
  std::mutex conversation_mutex;

  Renderer renderer;
  std::thread renderer_thread;

  std::string message = "Hello world!";
  std::mutex message_mutex;

  std::string input = "";
  std::mutex input_mutex;

  void run();

  void start_renderer();
};

#endif /* CONNECTION_HPP */
