FROM ubuntu:latest

WORKDIR /opt/tlsClient

RUN apt-get clean && apt-get update --allow-insecure-repositories
RUN apt-get install -y telnet net-tools --allow-unauthenticated

COPY ./release/ca.crt /opt/tlsClient/cert/
COPY ./release/client.key /opt/tlsClient/cert/
COPY ./release/expired_client.crt /opt/tlsClient/cert/client.crt
COPY ./release/tlsclient /opt/tlsClient/