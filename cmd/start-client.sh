#!/bin/sh
if [ "$1" = "1" ]
then
  docker-compose exec tls-client1 ./tlsclient 10.0.2.8
elif [ "$1" = "2" ]
then
  docker-compose exec tls-client2 ./tlsclient 10.0.2.8
elif [ "$1" = "ex" ]
then
  docker-compose exec tls-client-expired ./tlsclient 10.0.2.8
fi