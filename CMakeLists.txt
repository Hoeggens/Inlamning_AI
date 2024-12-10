cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ContactBookApp)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include MySQL directories
include_directories(/usr/include/mysql)
link_directories(/usr/lib/x86_64-linux-gnu)

# Add the executable
add_executable(ContactBookApp main.cpp contactbook.cpp)

# Link the MySQL client library
target_link_libraries(ContactBookApp mysqlclient)
