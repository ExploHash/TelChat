#ifndef CONVERSATION_MESSAGE_HPP
#define CONVERSATION_MESSAGE_HPP

#include <string>

struct conversation_message {
  std::string text;
  std::string sender_name;
  std::string receiver_name;
  std::string timestamp;
};

#endif /* CONVERSATION_MESSAGE_HPP */
