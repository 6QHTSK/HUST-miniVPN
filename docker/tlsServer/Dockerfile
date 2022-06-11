FROM ubuntu:impish
MAINTAINER 6QHTSK "psk2019@qq.com"
WORKDIR /opt/tlsServer

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list && apt-get clean && apt-get update --allow-insecure-repositories
RUN apt-get install -y telnet net-tools --allow-unauthenticated
RUN useradd test && echo 'test:123456' | chpasswd

COPY ./cert/ca.crt /opt/tlsServer/cert/
COPY ./cert/server.key /opt/tlsServer/cert/
COPY ./cert/server.crt /opt/tlsServer/cert/
COPY ./release/tlsserver /opt/tlsServer/
COPY ./lib/ /opt/tlsServer/lib/