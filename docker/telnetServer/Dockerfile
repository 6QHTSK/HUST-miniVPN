FROM ubuntu:latest
MAINTAINER 6QHTSK "psk2019@qq.com"
WORKDIR /opt

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list
RUN apt-get clean
RUN apt-get update
RUN apt-get install -y openbsd-inetd telnetd net-tools telnet
RUN useradd test && echo 'test:123456' | chpasswd

COPY ./docker/telnetServer/entrypoint.sh .