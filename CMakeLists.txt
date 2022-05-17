cmake_minimum_required(VERSION 3.22)
project(miniVPN)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_RPATH lib)

find_package(OpenSSL REQUIRED)
#if(OPENSSL_FOUND)
#    include_directories(${OPENSSL_INCLUDE_DIRS})
#    message(STATUS "OpenSSL Found!")
#endif()

add_executable(tlsclient tlsclient.cpp src/Client.cpp src/Tun/Tun.cpp src/Tun/IoctlIfSock.cpp src/Sock/SockBase.cpp src/Sock/SockClient.cpp src/Epoll/Epoll.cpp src/Model/Packet.h)
add_executable(tlsserver tlsserver.cpp src/Server.cpp src/Tun/Tun.cpp src/Tun/IoctlIfSock.cpp src/Sock/SockBase.cpp src/Sock/SockServer.cpp src/Epoll/Epoll.cpp src/Model/Packet.h)

include_directories(${OPENSSL_INCLUDE_DIRS})
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(DEP_LIBS OpenSSL::Crypto OpenSSL::SSL)

target_link_libraries(tlsclient ${DEP_LIBS})
target_link_libraries(tlsserver ${DEP_LIBS})