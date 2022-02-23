FROM ubuntu:latest
RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y \
    gcc \
    make \
    git \
    binutils \
    libc6-dev \
    gdb \
    sudo

ARG USER="cc"
RUN useradd ${USER}  && \
    mkdir -p /home/${USER} && \ 
    chown ${USER} /home/${USER}
COPY --chown=${USER} . /home/${USER}/

USER ${USER}
WORKDIR /home/${USER}