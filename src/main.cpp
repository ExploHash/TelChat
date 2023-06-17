#include <iostream>
//Include listener.cpp
#include "listener.cpp"

int main() {
    std::cout << "Hello World!";

    // Create listener
    Listener listener;

    // Listen for connections
    listener.listen_for_connections();
}