# gcc -static exploit.c -o rfs/home/exploit -lseccomp
gcc exploit.c -o rfs/home/user/libxpl.so --shared
rm initramfs.cpio.gz
cd rfs
find . -print0 | cpio --null -ov --format=newc > initramfs.cpio
gzip -c ./initramfs.cpio > ../initramfs.cpio.gz

