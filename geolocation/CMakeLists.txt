cmake_minimum_required(VERSION 3.0)

# set the project name
project(prepare)
project(locate)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

# add the executable
add_executable(prepare prepare_app.cpp)
add_executable(locate locate_app.cpp)

target_link_libraries(prepare PRIVATE Threads::Threads)
