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
  std::list <std::unique_ptr<ConnectionState> > connections;
  std::queue <connection_manager_message>* messages;
  std::mutex* messages_mutex;
  void run();
private:
  void check_for_manager_messages();
  void check_for_connection_messages();
  void handle_manager_message(connection_manager_message message);
  void handle_connection_message(connection_message message, ConnectionState* current_connection_state);
  void spawn_new_connection(int socket_fd, std::queue <connection_message>* messages_to_manager, std::mutex* messages_to_manager_mutex, std::queue <connection_message>* messages_to_connection, std::mutex* messages_to_connection_mutex);
  ConnectionState* find_connection_state_by_username(std::string username);
  ConnectionState* find_connection_state_by_username_connected_to(std::string username);
};

#endif /* CONNECTION_MANAGER_HPP */