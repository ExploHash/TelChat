#include <iostream>
#include "../include/listener.hpp"

int main() {
  std::cout << "Hello World!" << std::endl;

  // Create listener
  Listener listener;

  // Run listener
  listener.run();
}