#include "../include/input_reader.hpp"

void InputReader::run() {
  while (true) {
    // Get input
    char input[1024];
    int read_bytes = read(socket_fd, input, 1024);

    if (read_bytes > 0) {
      // Create new string without \n
      std::string input_string(input);
      std::cout << "Got input: " << input_string << std::endl;
      std::regex newlineRegex("[\r\n]+$");
      input_string = std::regex_replace(input, newlineRegex, "");

      std::cout << "Got regex: " << input_string << std::endl;

      if (input_string.length() == 0) {
        continue;
      }

      std::scoped_lock<std::mutex> lock(*messages_mutex);
      messages->push(input_string);
      std::cout << "Pushed message to queue" << std::endl;
    }
  }
}