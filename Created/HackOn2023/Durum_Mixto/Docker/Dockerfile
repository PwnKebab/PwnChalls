FROM ubuntu:20.04

RUN apt update && apt full-upgrade -y && DEBIAN_FRONTEND="noninteractive" TZ="America/New_York" apt install xinetd build-essential libpthread-stubs0-dev ssh tzdata nano -y 
RUN mkdir /var/run/sshd
RUN sed 's@session\srequired\spam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
RUN useradd -rm -d /home/user -s /bin/bash user
WORKDIR /home/user

COPY challenge.c challenge.c
COPY flag.txt flag.txt
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN gcc challenge.c -o chall -masm=intel
RUN chown root:root chall
RUN chmod +s chall
RUN chown root:root flag.txt
RUN chmod 600 flag.txt
COPY setup.sh setup.sh
RUN chmod +x setup.sh
RUN service ssh start

EXPOSE 22
CMD ["/home/user/setup.sh"]
