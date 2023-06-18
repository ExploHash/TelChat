#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <list>
#include <queue>
#include <thread>
#include <mutex>
#include "connection_state.hpp"
#include "connection.hpp"

enum class ConnectionManagerMessageType {
  NEW_CONNECTION,
};

struct connection_manager_message {
  ConnectionManagerMessageType type;
  int socket;
};

class ConnectionManager
{
public:
  std::list <std::unique_ptr<ConnectionState>> connections;
  std::queue <connection_manager_message> * messages;
  std::mutex * messages_mutex;
  void run();
private:
  void check_for_manager_messages();
  void check_for_connection_messages();
  void handle_manager_message(connection_manager_message message);
  void handle_connection_message(connection_message message);
  void spawn_new_connection(int socket_fd, std::queue <connection_message> * messages, std::mutex * messages_mutex);
};

#endif /* CONNECTION_MANAGER_HPP */