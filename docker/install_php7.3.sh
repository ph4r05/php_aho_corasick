#!/bin/bash
export DEBIAN_FRONTEND=noninteractive

apt-get remove --yes php5.6-dev
apt-get remove --yes php7.2-dev

apt-get update
apt-get install --yes software-properties-common
add-apt-repository ppa:ondrej/php
apt-get update

apt-get install --no-install-recommends php7.3-dev
