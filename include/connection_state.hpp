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
  INPUT_RECEIVED, // Server received input from client
  WANTS_LEAVE, // Client wants to leave conversation
  SHUTDOWN, // Server wants conversation to shutdown
  SHUTDOWN_SUCCESSFULL // Client successfully shut down
};

struct connection_message {
  ConnectionMessageType type;
  std::string text;
};

class ConnectionState {
public:
  bool is_connected = false;
  bool is_active = true;
  std::string username;
  std::string username_connected_to;
  std::queue<connection_message> messages_to_manager;
  std::mutex messages_to_manager_mutex;
  std::queue<connection_message> messages_to_connection;
  std::mutex messages_to_connection_mutex;
  std::thread connection_thread;
};

#endif /* CONNECTION_STATE_HPP */
