#   Classpoly Docker file
# -----------------------------------------------------------------------------
# Creates compilation image, i.e. you can modify the built sources.
# However, binaries are also compiled during image creation
# thus available in the image.

# Base image is an argument - debian or ubuntu.
ARG BASE_IMAGE=ubuntu:latest

# Builder stage
# Multistage docker build, requires docker 17.05
# https://docs.docker.com/develop/develop-images/dockerfile_best-practices/
# https://docs.docker.com/engine/reference/builder/
# https://medium.com/@tonistiigi/advanced-multi-stage-build-patterns-6f741b852fae
FROM ${BASE_IMAGE} AS base

ARG DEVEL_TOOLS=0
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Prague

RUN set -ex && \
    apt-get update && \
    apt-get --no-install-recommends --yes install \
        autoconf \
        automake \
        bzip2 \
        ca-certificates \
        curl \
        g++ \
        git \
        libtool-bin \
        make \
        php7.2-dev \
        pkg-config \
        python \
        rsync \
        unzip \
        wget \
    && if [ "${DEVEL_TOOLS}" -eq 1 ] ; then \
       apt-get --no-install-recommends --yes install \
          gdb \
          gdbserver \
          software-properties-common \
          valgrind \
          vim; \
       fi \
    && rm -rf /var/lib/apt/lists/*


# Building class poly
FROM base AS builder
WORKDIR /usr/local/src

ENV PROJECT_DIR /usr/local/src/php_aho_corasick

# Build either from current source or github repo (no local files needed then)
ARG DIR_BUSTER=0

# php_aho_corasick
COPY config* $PROJECT_DIR/
COPY build.sh $PROJECT_DIR/
COPY src/ $PROJECT_DIR/src/

COPY docker/*.sh /usr/local/bin/
RUN set -ex \
    && chmod +x /usr/local/bin/*.sh

WORKDIR $PROJECT_DIR
RUN set -ex \
    && phpize --clean \
    && phpize \
    && ./configure --enable-ahocorasick \
    && make clean \
    && make

COPY examples/ $PROJECT_DIR/examples/

