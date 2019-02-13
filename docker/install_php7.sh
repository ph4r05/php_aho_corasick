#!/bin/bash
export DEBIAN_FRONTEND=noninteractive

apt-get remove --yes php5.6-dev
apt-get install --no-install-recommends php7.2-dev
