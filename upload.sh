docker cp cmake-build-debug/tlsclient HostU:/home
docker cp lib HostU:/home/

docker cp cmake-build-debug/tlsclient HostV:/home
docker cp lib HostV:/home/

docker cp cmake-build-debug/tlsserver Gateway:/home
docker cp lib Gateway:/home/