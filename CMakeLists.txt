cmake_minimum_required(VERSION 3.22)
project(miniVPN)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_RPATH lib)

find_package(OpenSSL REQUIRED)
#if(OPENSSL_FOUND)
#    include_directories(${OPENSSL_INCLUDE_DIRS})
#    message(STATUS "OpenSSL Found!")
#endif()

add_executable(tlsclient tlsclient.cpp src/Client.cpp src/Tun/Tun.cpp src/Tun/TunController.cpp src/Client/SSLClient.cpp src/Epoll/Epoll.cpp src/Model/Packet.h)
add_executable(tlsserver tlsserver.cpp src/Server.cpp src/Tun/Tun.cpp src/Tun/TunController.cpp src/Server/SSLServer.cpp src/Epoll/Epoll.cpp src/Model/Packet.h src/Server/VirtualAddress.cpp src/Server/VirtualAddress.h src/Server/InterfaceInfo.cpp src/Server/InterfaceInfo.h)

include_directories(${OPENSSL_INCLUDE_DIRS})
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(DEP_LIBS OpenSSL::Crypto OpenSSL::SSL crypt)

target_link_libraries(tlsclient ${DEP_LIBS})
target_link_libraries(tlsserver ${DEP_LIBS})