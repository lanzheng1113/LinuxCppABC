cmake_minimum_required(VERSION 3.5)

project(test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message("--------------------OPENSSL BEG----------------------")
find_package(OpenSSL REQUIRED)
message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL lib: ${OPENSSL_LIBRARIES}")
message("--------------------OPENSSL END----------------------")
add_executable(ssl_server main.cpp)
target_link_libraries(ssl_server ssl crypto)
