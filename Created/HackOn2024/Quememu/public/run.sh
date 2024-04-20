#! /bin/bash

/home/user/qemu-system-x86_64 \
	-nographic \
	-kernel /home/user/vmlinuz-5.15.0-92-generic \
	-initrd /home/user/initramfs.cpio.gz \
	-device quememu \
	-m 1024M \
	-L /home/user/pc-bios/ \
	-no-reboot \
	-monitor none \
	-sandbox on,obsolete=deny,elevateprivileges=deny,spawn=deny,resourcecontrol=deny \
	-append "console=ttyS0 oops=panic panic=1 quiet"
