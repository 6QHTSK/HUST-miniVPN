FROM ubuntu:latest
MAINTAINER 6QHTSK "psk2019@qq.com"
WORKDIR /opt/tlsClient

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list
RUN apt-get clean
RUN apt-get update
RUN apt-get install -y telnet net-tools

COPY ./cert/ca.crt /opt/tlsClient/cert/
COPY ./cert/client.key /opt/tlsClient/cert/
COPY ./cert/expired_client.crt /opt/tlsClient/cert/client.crt
COPY ./release/tlsclient /opt/tlsClient/
COPY ./lib/ /opt/tlsClient/lib/
