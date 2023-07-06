FROM ubuntu:latest

WORKDIR /opt

COPY ./app/get_source.sh ./get_source.sh

RUN bash ./get_source.sh

RUN apt-get clean && \
    apt-get update --allow-insecure-repositories

RUN apt-get install -y openbsd-inetd telnetd net-tools telnet --allow-unauthenticated
RUN useradd test && echo 'test:123456' | chpasswd

COPY ./docker/telnetServer/entrypoint.sh .