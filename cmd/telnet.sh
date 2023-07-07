#!/bin/sh
if [ "$1" = "1" ]
then
  docker-compose exec tls-client1 telnet 192.168.60.101
elif [ "$1" = "2" ]
then
  docker-compose exec tls-client2 telnet 192.168.60.101
fi