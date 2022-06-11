#!/bin/sh

echo "" > /etc/resolv.conf
/etc/init.d/openbsd-inetd start
route del default gw 192.168.60.100
route add default gw 192.168.60.1
sh
