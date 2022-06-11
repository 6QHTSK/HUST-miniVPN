FROM ubuntu:impish
MAINTAINER 6QHTSK "psk2019@qq.com"
WORKDIR /opt/tlsClient

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list && apt-get clean && apt-get update --allow-insecure-repositories
RUN apt-get install -y telnet net-tools --allow-unauthenticated

COPY ./cert/ca.crt /opt/tlsClient/cert/
COPY ./cert/client.key /opt/tlsClient/cert/
COPY ./cert/client.crt /opt/tlsClient/cert/
COPY ./release/tlsclient /opt/tlsClient/
COPY ./lib/ /opt/tlsClient/lib/