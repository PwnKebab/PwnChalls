#!/bin/bash

echo "user:$PASSWORD" | chpasswd
/usr/sbin/sshd -D -f /etc/ssh/sshd_config >> /tmp/logs
