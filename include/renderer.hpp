#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <mutex>
#include <thread>
#include <unistd.h>
#include <string>
#include "conversation.hpp"

enum class RendererState {
  MESSAGE,
  INPUT,
  CHAT
};

class Renderer {
public:
  int socket_fd;
  
  Conversation* conversation;
  std::mutex* conversation_mutex;

  std::string* message;
  std::mutex* message_mutex;

  std::string* input;
  std::mutex* input_mutex;

  RendererState* state;
  std::mutex* state_mutex;

  void run();
  void render_message();
  void render_input();
  void render_chat();
};

#endif /* RENDERER_HPP */
