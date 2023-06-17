#include "../include/connection.hpp"
#include <iostream>

void Connection::run() {
  // Start renderer
  renderer_thread = std::thread(&Connection::start_renderer, this, socket_fd);

  // Listen for input
  listen_for_input();
}

void Connection::start_renderer(int socket_fd) {
  // Initialize renderer
  Renderer renderer;
  renderer.socket_fd = socket_fd;
  renderer.conversation = &conversation;
  renderer.conversation_mutex = &conversation_mutex;
  renderer.message = &message;
  renderer.message_mutex = &message_mutex;
  renderer.input = &input;
  renderer.input_mutex = &input_mutex;
  renderer.state = &state;
  renderer.state_mutex = &state_mutex;

  // Run renderer
  renderer.run();
}

void Connection::listen_for_input() {
  std::string input = "";

  while (true)
  {
    // Read input
    std::cout << "Reading input" << std::endl;
    char buffer[1024] = { 0 };
    read(socket_fd, buffer, 1024);
    std::cout << "Read input" << std::endl;

    // Loop through buffer
    for (int i = 0; i < sizeof(buffer); i++)
    {
      switch (buffer[i])
      {
        // Check for backspace
        case 127:
          // Remove last character
          input.pop_back();
          break;
        // Check for enter
        case 10:
          //TODO: Send message
          break;
        default:
          // Add character to input
          input += buffer[i];
          break;
      }
    }

    // Lock input mutex
    input_mutex.lock();

    // Copy input to mutex
    this->input = input;

    std::cout << input << std::endl;

    // Unlock input mutex
    input_mutex.unlock();
  }
}