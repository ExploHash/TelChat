#ifndef INPUT_READER_HPP
#define INPUT_READER_HPP

#include <queue>
#include <mutex>
#include "connection_state.hpp"
#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>

enum class InputReaderMessageType {
  INPUT_RECEIVED,
  SHUTDOWN,
  SHUTDOWN_SUCCESSFUL
};

struct InputReaderMessage {
  std::string text;
  InputReaderMessageType type;
};

class InputReader {
  public: 
    int socket_fd;
    bool is_running = true;
    std::queue <InputReaderMessage> * messages_from_reader;
    std::mutex * messages_from_reader_mutex;

    std::queue <InputReaderMessage> * messages_to_reader;
    std::mutex * messages_to_reader_mutex;
  
    void run();
    void check_connection_messages();
};

#endif /* INPUT_READER_HPP */