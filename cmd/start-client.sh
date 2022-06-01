#!/bin/sh
if [ "$1" = "1" ]
then
  docker exec -it minivpn-tls-client1-1 ./tlsclient 10.0.2.8
elif [ "$1" = "2" ]
then
  docker exec -it minivpn-tls-client2-1 ./tlsclient 10.0.2.8
elif [ "$1" = "ex" ]
then
  docker exec -it minivpn-tls-client-expired-1 ./tlsclient 10.0.2.8
fi