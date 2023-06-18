#include "../include/renderer.hpp"
#include <iostream>

void Renderer::empty_screen() {
  std::string empty_screen = "\033[2J\033[1;1H";
  write(socket_fd, empty_screen.c_str(), sizeof(empty_screen));
};

void Renderer::render_message(std::string message) {
  empty_screen();

  // Write message
  write(socket_fd, message.c_str(), sizeof(message));
};

void Renderer::render_input(std::string question) {
  empty_screen();

  // Write question
  write(socket_fd, question.c_str(), sizeof(question));
};

void Renderer::render_chat(std::list <conversation_message> messages, std::string input) {
  // Loop through messages
  for (auto const& message : messages)
  {
    // Write message
    write(socket_fd, message.sender_name.c_str(), sizeof(message.sender_name));
    write(socket_fd, ": ", sizeof(": "));
    write(socket_fd, message.text.c_str(), sizeof(message.text));
    write(socket_fd, "\n", sizeof("\n"));
  }

  // Write line
  std::string line = "----------------------------------------\n";
  write(socket_fd, line.c_str(), sizeof(line));

  // Write input
  std::string input_prompt = "You: ";
  write(socket_fd, input_prompt.c_str(), sizeof(input_prompt));
};