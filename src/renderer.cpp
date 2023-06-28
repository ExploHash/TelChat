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
    // Save cursor position
    std::string save_cursor_position = "\033[s";
    write(socket_fd, save_cursor_position.c_str(), save_cursor_position.size());
    // Move cursor to 0,0
    std::string move_cursor_up = "\033[0;0H";
    write(socket_fd, move_cursor_up.c_str(), move_cursor_up.size());
    // Clear 6 lines
    for (int i = 0; i < 6; i++) {
      std::string clear_line = "\033[K\n";
      write(socket_fd, clear_line.c_str(), clear_line.size());
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(1000));


  // Render saved messages
  std::string move_cursor_up = "\033[0;0H";
  write(socket_fd, move_cursor_up.c_str(), move_cursor_up.size());

  for (auto message : messages) {
    // Write message
    std::string message_text = message.sender_name + ": " + message.text + "\n";
    write(socket_fd, message_text.c_str(), message_text.size());
    // Send to console as well
    std::cout << message_text.c_str() << std::endl;
  }


  // Write input
  if (!preserve_input) {
    // Move cursor to 6,0
    std::string move_cursor_up = "\033[7;0H";
    write(socket_fd, move_cursor_up.c_str(), move_cursor_up.size());

    // Write line
    std::string line = "---------\n";
    write(socket_fd, line.c_str(), line.size());

    // Write options
    std::string options = ":quit <to quit>\n";
    write(socket_fd, options.c_str(), options.size());
  

    std::string input_prompt = "You: ";
    write(socket_fd, input_prompt.c_str(), input_prompt.size());
  } else {
    // Restore cursor position
    std::string restore_cursor_position = "\033[u";
    write(socket_fd, restore_cursor_position.c_str(), restore_cursor_position.size());
  }
};