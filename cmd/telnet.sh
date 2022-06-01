#!/bin/sh
if [ "$1" = "1" ]
then
  docker exec -it minivpn-tls-client1-1 telnet 192.168.60.101
elif [ "$1" = "2" ]
then
  docker exec -it minivpn-tls-client2-1 telnet 192.168.60.101
fi