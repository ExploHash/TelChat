#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <mutex>
#include <thread>
#include <unistd.h>
#include <string>
#include <list>
#include "conversation_message.hpp"

class Renderer {
public:
  int socket_fd;

  void render_message(std::string message);
  void render_input(std::string question);
  void render_chat(std::list <conversation_message> messages, std::string input);
  void empty_screen();
};

#endif /* RENDERER_HPP */
