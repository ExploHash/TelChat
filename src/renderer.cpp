#include "../include/renderer.hpp"
#include <iostream>

void Renderer::run() {
  while (true)
  {
    // Clear connection screen
    std::string clear = "\033[2J\033[1;1H";
    write(socket_fd, clear.c_str(), sizeof(clear));

    // Lock state mutex
    state_mutex->lock();

    // Copy state
    RendererState state_copy = *this->state;

    // Unlock state mutex
    state_mutex->unlock();
    
    switch (state_copy)
    {
    case RendererState::MESSAGE:
      render_message();
      break;
    case RendererState::INPUT:
      render_input();
      break;
    case RendererState::CHAT:
      render_chat();
      break;
    default:
      break;
    }
    

    // Sleep for 50ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void Renderer::render_message() {
  // Lock message mutex
  message_mutex->lock();

  // Write message from mutex
  write(socket_fd, message->c_str(), sizeof(*message));

  // Unlock message mutex
  message_mutex->unlock();
};

void Renderer::render_input() {
  // Write question
  std::string question = "What is your name? ";
  write(socket_fd, question.c_str(), sizeof(question));

  // Lock input mutex
  input_mutex->lock();

  // Write input from mutex
  write(socket_fd, input->c_str(), sizeof(*input));

  // Unlock input mutex
  input_mutex->unlock();
};

void Renderer::render_chat() {
  // Lock conversation mutex
  // std::scoped_lock<std::mutex> lock(*conversation_mutex);

  // Write conversation
  // write(socket_fd, conversation->get().c_str(), sizeof(conversation->get()));
};