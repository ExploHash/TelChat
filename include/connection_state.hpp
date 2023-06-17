#ifndef CONNECTION_STATE_HPP
#define CONNECTION_STATE_HPP

#include <string>
#include <queue>
#include <mutex>

enum class ConnectionMessageType {
  MESSAGE,
  USERNAME,
  USERNAME_CONNECTED_TO
};

struct connection_message {
  ConnectionMessageType type;
  std::string text;
};

class ConnectionState {
public:
  bool is_authenticated = false;
  std::string username;
  std::string username_connected_to;
  std::queue<connection_message> messages;
  std::mutex messages_mutex;

  void add_message(ConnectionMessageType type, std::string message_text);
};

#endif /* CONNECTION_STATE_HPP */
