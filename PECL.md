## PECL publishing

```bash
docker build -t="ahoc" --build-arg DEVEL_TOOLS=1 .
docker run -i --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --mount type=bind,src=`pwd`,dst=/aho -t ahoc

cd /aho
apt-get update
apt-get install php7.2-xml

phpize --clean
phpize
./configure --enable-ahocorasick
make clean
make

wget http://pear.php.net/go-pear.phar
php go-pear.phar

pear package

# Upload here: https://pecl.php.net/release-upload.php
```


## Debugging

Installing pyenv in the Docker

```
. docker/install_penv.sh

NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make test TESTS="--show-diff"
```
