cmake_minimum_required(VERSION 3.16)
project(VietnamOnline VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Shared header-only library (client + server)
add_library(vno_shared INTERFACE)
target_include_directories(vno_shared INTERFACE
    ${CMAKE_SOURCE_DIR}/shared/include
)

# Console prototype client (MVP playable)
add_executable(vno_prototype
    client/prototype/src/main.cpp
    client/prototype/src/GameEngine.cpp
)
target_include_directories(vno_prototype PRIVATE
    client/prototype/include
)
target_link_libraries(vno_prototype PRIVATE vno_shared)

# MVP game server
add_subdirectory(server)

# Tests
enable_testing()
add_subdirectory(tests)
