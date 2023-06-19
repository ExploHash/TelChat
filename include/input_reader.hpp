#ifndef INPUT_READER_HPP
#define INPUT_READER_HPP

#include <queue>
#include <mutex>
#include "connection_state.hpp"
#include <unistd.h>
#include <iostream>
#include <regex>

class InputReader {
  public: 
    int socket_fd;
    std::queue <std::string> * messages;
    std::mutex * messages_mutex;
  
    void run();
};

#endif /* INPUT_READER_HPP */