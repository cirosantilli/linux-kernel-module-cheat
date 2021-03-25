# https://cirosantilli.com/linux-kernel-module-cheat#docker
FROM ubuntu:20.04
COPY setup /
COPY requirements.txt /
RUN /setup -y
CMD bash
