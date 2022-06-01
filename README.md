# miniVPN - HUST网络安全实验3

一个简单的基于SSL的应用层VPN实践

## 项目特点

1. 双重认证，在验证SSL证书基础上拥有密码验证。
2. 开箱即用，自动进行设备的启动，客户端无需配置路径；自动代理内网网段，客户端无需配置路由。
3. 友好的提示，服务端可以显示当前连接日志、流量，以及可供连接的IP。
4. 快速的实验环境搭建，只需`docker compose up -d`即可搭建实验环境

## 编译运行

### 项目依赖与编译环境

1. 使用openssl-1.1.1库。
2. 使用cmake生成可执行文件
3. 使用了docker和docker-compose

### 编译项目
```shell
make all
```
### 运行项目
```shell
docker compose up -d
```

### 简便指令

```shell
sh ./cmd/start-server.sh # 启动TLS服务器
sh ./cmd/start-client.sh (1/2/ex) # 启动TLS客户端 （客户端1,客户端2,已过期客户端）
sh ./cmd/telnet.sh (1/2) # telnet连接内网主机
```

## 网络结构

### extranet (10.0.2.0/24)

| 容器名                | IP地址       |
|--------------------|------------|
| tls-server         | 10.0.2.8   |
| tls-client1        | 10.0.2.10  |
| tls-client2        | 10.0.2.11  |
| tls-client-expired | 10.0.2.12  |
| host               | 10.0.2.100 |

### intranet (192.168.60.0/24)

| 容器名                | IP地址           |
|--------------------|----------------|
| tls-server         | 192.168.60.1   |
| host               | 192.168.60.100 |
| telnet-server      | 192.168.60.101 |

## Tip & WIP

1. TLS 服务端运行在55555端口，不可更改
2. 客户读暂未支持证书验证错误原因反馈，但其可知晓其拥有的证书错误
3. 虚拟地址网段为192.168.53.0/24,暂不可更改
4. This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/)

## LICENSE

本项目采用MIT开源许可证