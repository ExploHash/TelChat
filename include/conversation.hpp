#ifndef CONVERSATION_HPP
#define CONVERSATION_HPP

#include <string>
#include <list>

struct conversation_message {
  std::string text;
  std::string sender_name;
  std::string receiver_name;
  std::string timestamp;
};

class Conversation {
public:
  bool active = false;
  std::list<conversation_message> messages;

  void add_message(std::string name, std::string message_text);
};

#endif /* CONVERSATION_HPP */
