# Description: Change source list to aliyun
if [ "$(uname -m)" == "aarch64" ]; then
  sed -i s@/ports.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list
else
  sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list
fi
