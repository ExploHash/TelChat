#include "../include/input_reader.hpp"

void InputReader::run() {
  while (true) {
    std::cout << "InputReader running" << std::endl;
    // Check for connection messages
    check_connection_messages();

    if (!is_running) break;

    // Get input
    char input[1024];
    int read_bytes = read(socket_fd, input, 1024);

    if (read_bytes > 0) {
      // Create new string without \n
      std::string input_string(input);

      //Cut until \n
      std::size_t newlinePos = input_string.find('\n');
      input_string = input_string.substr(0, newlinePos);

      // Remove any more newlines
      std::regex newlineRegex("[\\r\\n]+$");
      input_string = std::regex_replace(input_string, newlineRegex, "");
      

      if (input_string.length() == 0) {
        continue;
      }


      // Push message to queue
      std::scoped_lock<std::mutex> lock(*messages_from_reader_mutex);
      InputReaderMessage message;
      message.type = InputReaderMessageType::INPUT_RECEIVED;
      message.text = input_string;

      messages_from_reader->push(message);
      std::cout << "Pushed message to queue" << std::endl;

      // Sleep for a bit
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void InputReader::check_connection_messages() {
  // std::cout << "Checking connection messages" << std::endl;
  std::scoped_lock<std::mutex> lock(*messages_to_reader_mutex);

  // std::cout << "Messages to reader size: " << messages_to_reader->size() << std::endl;
  while (!messages_to_reader->empty()) {
    InputReaderMessage message = messages_to_reader->front();
    messages_to_reader->pop();

    std::cout << "Handling message from input reader" << std::endl;

    if (message.type == InputReaderMessageType::SHUTDOWN) {
      std::cout << "Shutting down input reader" << std::endl;
      is_running = false;

      // Send shutdown successful message
      InputReaderMessage message;
      message.type = InputReaderMessageType::SHUTDOWN_SUCCESSFUL;
      std::scoped_lock<std::mutex> lock(*messages_from_reader_mutex);
      messages_from_reader->push(message);

      std::cout << "Input reader shut down" << std::endl;
    }
  }
}