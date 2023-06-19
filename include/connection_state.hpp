#ifndef CONNECTION_STATE_HPP
#define CONNECTION_STATE_HPP

#include <string>
#include <queue>
#include <mutex>
#include <thread>

enum class ConnectionMessageType {
  MESSAGE_SEND, // Client wants to send a message
  MESSAGE_RECEIVE, // Server wants to send a message
  USERNAME, // Client chose a username
  CONVERSATION_WANTS, // Client wants to connect to a username
  CONVERSATION_ACCEPTED, // Server accepted the conversation
  INPUT_RECEIVED // Server received input from client
};

struct connection_message {
  ConnectionMessageType type;
  std::string text;
};

class ConnectionState {
public:
  bool is_connected = false;
  std::string username;
  std::string username_connected_to;
  std::queue<connection_message> messages;
  std::mutex messages_mutex;
  std::thread connection_thread;
};

#endif /* CONNECTION_STATE_HPP */
