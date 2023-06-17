#include "../include/renderer.hpp"
#include <iostream>

void Renderer::run(int socket_fd) {
  //Log socket fd
  std::cout << "Socket fd renderer1: " << socket_fd << std::endl;
  // Set socket fd
  this->socket_fd = socket_fd;

  while (true)
  {
    switch (state)
    {
    case RendererState::MESSAGE:
      render_message();
      break;
      // case RendererState::INPUT:
      //   render_input();
      //   break;
      // case RendererState::CHAT:
      //   render_chat();
      //   break;
    default:
      break;
    }

    // Sleep for 50ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void Renderer::render_message() {
  // Lock message mutex
  // std::scoped_lock<std::mutex> lock(*message_mutex);

  // Write message2
  std::cout << "Socket fd renderer: " << socket_fd << std::endl;
  // write(socket_fd, message2.c_str(), sizeof(message2));
}