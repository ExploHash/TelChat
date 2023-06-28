#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unistd.h>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include "renderer.hpp"
#include "connection_state.hpp"
#include "input_reader.hpp"

enum class ConnectionStatus {
  SHOW_MESSAGE,
  CHOOSE_USERNAME,
  CHOOSE_PERSON,
  CHAT
};

class Connection {
public:
  int socket_fd;
  std::string username;
  std::string username_connected_to;
  std::string message_to_show;
  bool initiated_shutdown = false;
  bool is_running = true;

  Renderer renderer;

  bool render_needed = true;
  bool chat_preserve_input = false;

  ConnectionStatus status = ConnectionStatus::CHOOSE_USERNAME;
  
  std::list <conversation_message> conversation_messages; // Only keep the last 7 messages

  std::queue <connection_message> * messages_to_connection;
  std::mutex * messages_to_connection_mutex;

  std::queue <connection_message> * messages_to_manager;
  std::mutex * messages_to_manager_mutex;

  std::queue <InputReaderMessage> messages_from_input_reader;
  std::mutex messages_from_input_reader_mutex;

  std::queue <InputReaderMessage> messages_to_input_reader;
  std::mutex messages_to_input_reader_mutex;

  std::thread input_reader_thread;

  void run();
  void check_connection_messages();
  void check_connection_input_messages();
  void handle_connection_message(connection_message message);
  void add_conversation_message(std::string message, std::string sender_name);
  void send_message(std::string message);
  void send_username(std::string username);
  void send_leave();
  void render();
  void handle_input(std::string input);
  void spawn_input_reader();
  void shutdown_prepare();
  void shutdown();
};

#endif /* CONNECTION_HPP */
