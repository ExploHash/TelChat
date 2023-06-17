#include "../include/connection_state.hpp"

void ConnectionState::add_message(ConnectionMessageType type, std::string message_text) {
  // Create new message
  connection_message new_message;
  new_message.type = type;
  new_message.text = message_text;
  // Add message to list
  std::scoped_lock lock(messages_mutex);
  messages.push(new_message);
}
