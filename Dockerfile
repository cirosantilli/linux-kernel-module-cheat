# https://github.com/cirosantilli/linux-kernel-module-cheat#docker
FROM ubuntu:18.04
RUN apt update
# Minimum requirements to run ./build --download-dependencies
RUN apt-get install -y \
      git \
      python3 \
      python3-distutils \
    ;
CMD bash
