#include "../include/connection.hpp"
#include <iostream>

void Connection::run() {
  // Start renderer
  start_renderer();

  std::cout << "Socket fd conn: " << socket_fd << std::endl;

  // // Write hello world
  // write(socket_fd, message.c_str(), sizeof(message));    

  // // Close connection
  // close(socket_fd);
}

void Connection::start_renderer() {
  // Initialize renderer
  std::cout << "Socket fd conn2: " << socket_fd << std::endl;
  // Copy socket fd to renderer
  // renderer.conversation = &conversation;
  // renderer.conversation_mutex = &conversation_mutex;
  // renderer.message = &message;
  // renderer.message_mutex = &message_mutex;
  // renderer.input = &input;
  // renderer.input_mutex = &input_mutex;


  // Start renderer thread
  std::thread t1(&Renderer::run, &renderer, socket_fd);
  // renderer_thread = std::move(t1);
  t1.detach();
}
