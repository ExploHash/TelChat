#include "../include/conversation.hpp"

void Conversation::add_message(std::string name, std::string message_text) {
  // Create new message
  conversation_message new_message;
  new_message.sender_name = name;
  new_message.text = message_text;
  // Add message to list
  messages.push_back(new_message);
}