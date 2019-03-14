#!/bin/bash
# https://github.com/phpenv/phpenv

apt-get install libcurl4-openssl-dev libmcrypt-dev libreadline-dev \
    libxslt1-dev libxml2-dev libbz2-dev libjpeg-dev libpng-dev \
    libtidy-dev

# Installer for phpenv.
curl -L http://git.io/phpenv-installer | bash

export PHPENV_ROOT="/root/.phpenv"
if [ -d "${PHPENV_ROOT}" ]; then
  export PATH="${PHPENV_ROOT}/bin:${PATH}"
  eval "$(phpenv init -)"
fi

echo 'export PATH="$HOME/.phpenv/bin:$PATH"' >> ~/.bash_profile

echo 'eval "$(phpenv init -)"' >> ~/.bash_profile

exec $SHELL -l

# workaround for new ubuntu
cd /usr/include
ln -s x86_64-linux-gnu/curl
cd -

# phpenv
phpenv install --list
phpenv install 5.5.38
phpenv install 5.6.40
phpenv install 7.2.16

for PHP_PATH in $HOME/.phpenv/versions/[0-9].[0-9].[0-9]*; do
  PHP_VERSION=${PHP_PATH##*/};
  unlink "${HOME}/.phpenv/versions/${PHP_VERSION%.*}" 2>/dev/null
  ln -s "${PHP_PATH}" "${HOME}/.phpenv/versions/${PHP_VERSION%.*}" 2>/dev/null
done

phpenv rehash

phpenv global 5.6.40 2>/dev/null


