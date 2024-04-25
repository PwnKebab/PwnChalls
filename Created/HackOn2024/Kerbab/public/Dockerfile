FROM ubuntu:23.04

RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y \
xinetd libpixman-1-dev libepoxy-dev libspice-server-dev libfdt-dev \
libusb-1.0.0-dev libasound-dev libaio-dev libslirp-dev libgtk-3-0 \
libbrlapi-dev libcurl4-openssl-dev qemu-system

RUN apt-get clean
RUN useradd -m user

COPY pc-bios /home/user/pc-bios
COPY run.sh /home/user/run.sh
COPY vmlinuz-4.19.306 /home/user/vmlinuz-4.19.306
COPY initramfs.cpio.gz /home/user/initramfs.cpio.gz
COPY xinetd /etc/xinetd.d/ctf

RUN chown -R user:user /home/user/

USER user   
WORKDIR /home/user
CMD ["/usr/sbin/xinetd", "-dontfork"]
