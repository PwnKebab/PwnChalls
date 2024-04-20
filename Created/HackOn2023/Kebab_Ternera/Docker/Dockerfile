FROM ubuntu:20.04

RUN apt update && apt full-upgrade -y && apt install xinetd build-essential -y && apt install libseccomp-dev -y

COPY ./ctf.xinetd /etc/xinetd.d/ctf
COPY ./entrypoint.sh /start.sh
RUN echo "Blocked by ctf_xinetd" > /etc/banner_fail

RUN chmod +x /start.sh

COPY ./challenge.c /challenge.c
COPY ./flag /flag
RUN gcc /challenge.c -o /chall -lseccomp

CMD ["/start.sh"]

