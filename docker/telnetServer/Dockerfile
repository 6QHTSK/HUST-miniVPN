FROM ubuntu:latest

WORKDIR /opt

RUN apt-get clean && apt-get update --allow-insecure-repositories

RUN apt-get install -y openbsd-inetd telnetd net-tools telnet --allow-unauthenticated
RUN useradd test && echo 'test:123456' | chpasswd

COPY ./docker/telnetServer/entrypoint.sh .