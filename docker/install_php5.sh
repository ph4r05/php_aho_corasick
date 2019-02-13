#!/bin/bash
export DEBIAN_FRONTEND=noninteractive

apt-get remove --yes php7.2-dev
apt-get update
apt-get install software-properties-common
add-apt-repository ppa:ondrej/php
apt-get update
apt-get install --no-install-recommends --yes php5.6-dev
