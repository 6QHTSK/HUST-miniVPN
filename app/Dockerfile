FROM ubuntu:latest

WORKDIR /app

COPY . /app

RUN bash ./get_source.sh

RUN apt-get clean && \
    apt-get update --allow-insecure-repositories

RUN apt-get install -y cmake gcc g++ libssl-dev --allow-unauthenticated

RUN export OPENSSL_ROOT_DIR=/usr/include/openssl

# 生成app
RUN mkdir -p release && cd release && cmake .. && make

RUN rm -rf /app/release/*cmake* /app/release/*Makefile* /app/release/*CMake*

# 生成证书
RUN cd cert && make cert

# 转移证书文件到release目录
RUN cp cert/** release && rm release/Makefile