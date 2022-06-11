FROM ubuntu:impish
MAINTAINER 6QHTSK "psk2019@qq.com"
WORKDIR /opt

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list && apt-get clean && apt-get update --allow-insecure-repositories
RUN apt-get install -y openbsd-inetd telnetd net-tools telnet --allow-unauthenticated
RUN useradd test && echo 'test:123456' | chpasswd

COPY ./docker/telnetServer/entrypoint.sh .