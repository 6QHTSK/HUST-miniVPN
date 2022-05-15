cmake_minimum_required(VERSION 3.22)
project(miniVPN)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_RPATH lib)

find_package(OpenSSL REQUIRED)
#if(OPENSSL_FOUND)
#    include_directories(${OPENSSL_INCLUDE_DIRS})
#    message(STATUS "OpenSSL Found!")
#endif()

add_executable(tlsclient tlsclient.cpp Client/Client.cpp Tun/Tun.cpp Tun/IoctlIfSock.cpp Client/SockClient.cpp Epoll/Epoll.cpp Epoll/Epoll.h)
add_executable(tlsserver tlsserver.cpp Server/Server.cpp Tun/Tun.cpp Tun/IoctlIfSock.cpp Server/SockServer.cpp Epoll/Epoll.cpp Epoll/Epoll.h)

include_directories(${OPENSSL_INCLUDE_DIRS})
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(DEP_LIBS OpenSSL::Crypto OpenSSL::SSL)

target_link_libraries(tlsclient ${DEP_LIBS})
target_link_libraries(tlsserver ${DEP_LIBS})