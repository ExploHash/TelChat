#include "../include/renderer.hpp"
#include <iostream>

void Renderer::empty_screen() {
  std::string empty_screen = "\033[2J\033[1;1H";
  write(socket_fd, empty_screen.c_str(), empty_screen.size());
};

void Renderer::render_message(std::string message) {
  empty_screen();

  std::cout << message << std::endl;

  // Write message
  write(socket_fd, message.c_str(), message.size());
};

void Renderer::render_input(std::string question) {
  empty_screen();

  // Write question
  write(socket_fd, question.c_str(), question.size());
};

void Renderer::render_chat(std::list <conversation_message> messages, bool preserve_input) {
  if (!preserve_input) {
    empty_screen();
  }
  else {
    // Move cursor up 8 lines
    std::string move_cursor_up = "\033[8A";

    write(socket_fd, move_cursor_up.c_str(), move_cursor_up.size());
  }


  int rendered_messages = 0;

  // First render saved messages
  for (auto message : messages) {
    // Write message
    std::string message_text = message.sender_name + ": " + message.text + "\n";
    write(socket_fd, message_text.c_str(), message_text.size());

    rendered_messages++;
  }

  // Write remaining lines
  for (int i = rendered_messages; i < 7; i++) {
    std::string empty_line = "\n";
    write(socket_fd, empty_line.c_str(), empty_line.size());
  }

  // Write line
  std::string line = "----------------------------------------\n";
  write(socket_fd, line.c_str(), line.size());


  // Write input
  if (!preserve_input) {
    std::string input_prompt = "You: ";
    write(socket_fd, input_prompt.c_str(), input_prompt.size());
  }
};