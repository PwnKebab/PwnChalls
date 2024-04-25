#! /bin/bash

	qemu-system-x86_64 \
	-nographic \
	-cpu kvm64,+smep,+smap,check \
	-kernel /home/user/vmlinuz-4.19.306 \
	-initrd /home/user/initramfs.cpio.gz \
	-m 1024M \
	-L /home/user/pc-bios/ \
	-no-reboot \
	-monitor none \
	-sandbox on,obsolete=deny,elevateprivileges=deny,spawn=deny,resourcecontrol=deny \
	-append "console=ttyS0 oops=panic panic=1 quiet kaslr slub_debug=- apparmor=0" \
