FROM ubuntu:latest

WORKDIR /opt/tlsServer

COPY ./app/get_source.sh ./get_source.sh

RUN bash ./get_source.sh

RUN apt-get clean && \
    apt-get update --allow-insecure-repositories

RUN apt-get install -y telnet net-tools --allow-unauthenticated
RUN useradd test && echo 'test:123456' | chpasswd

COPY ./release/ca.crt /opt/tlsServer/cert/
COPY ./release/server.key /opt/tlsServer/cert/
COPY ./release/server.crt /opt/tlsServer/cert/
COPY ./release/tlsserver /opt/tlsServer/