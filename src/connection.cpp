#include <unistd.h>


class Connection {
public:
  int socket_fd;
  void run() {
    char hello_world[] = "Hello World!";
    write(socket_fd, hello_world, sizeof(hello_world));

    // Close connection
    close(socket_fd);
  }
};