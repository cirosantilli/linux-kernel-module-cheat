# https://cirosantilli.com/linux-kernel-module-cheat#docker
FROM ubuntu:18.04
COPY setup /
RUN /setup -y
CMD bash
