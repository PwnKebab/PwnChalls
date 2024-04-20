FROM ubuntu:23.04

RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y \
xinetd libpixman-1-dev libepoxy-dev libspice-server-dev libfdt-dev \
libusb-1.0.0-dev libasound-dev libaio-dev libslirp-dev libgtk-3-0 \
libbrlapi-dev libcurl4-openssl-dev

RUN apt-get clean
RUN useradd -m user

COPY flag /home/user/flag
COPY qemu-system-x86_64 /home/user/qemu-system-x86_64
COPY pc-bios /home/user/pc-bios
COPY run.sh /home/user/run.sh
COPY vmlinuz-5.15.0-92-generic /home/user/vmlinuz-5.15.0-92-generic
COPY initramfs.cpio.gz /home/user/initramfs.cpio.gz
COPY xinetd /etc/xinetd.d/maria

RUN chown -R user:user /home/user/

USER user   
WORKDIR /home/user
EXPOSE 1637
CMD ["/usr/sbin/xinetd", "-dontfork"]
