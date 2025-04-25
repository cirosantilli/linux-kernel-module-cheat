# https://cirosantilli.com/linux-kernel-module-cheat#docker
FROM ubuntu:20.04
COPY setup /
COPY requirements.txt /
# https://stackoverflow.com/questions/23513045/how-to-check-if-a-process-is-running-inside-docker-container/65942222#65942222
ENV LKMC_IN_DOCKER=true
RUN /setup -y
CMD bash
