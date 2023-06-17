#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <mutex>
#include <thread>
#include <unistd.h>
#include <string>

enum class RendererState {
  MESSAGE,
  INPUT,
  CHAT
};

class Renderer {
public:
  int socket_fd;
  RendererState state = RendererState::MESSAGE;
  // Conversation* conversation;
  // std::mutex* conversation_mutex;

  // std::string* message;
  // std::mutex* message_mutex;

  // std::string* input;
  // std::mutex* input_mutex;

  std::string message2 = "Hello world!";

  void run(int socket_fd);
  void render_message();
};

#endif /* RENDERER_HPP */
