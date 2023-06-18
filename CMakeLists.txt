cmake_minimum_required(VERSION 3.26)
project(TelChat)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

add_executable(TelChat src/main.cpp src/listener.cpp src/renderer.cpp src/connection.cpp src/connection_state.cpp src/connection_manager.cpp)