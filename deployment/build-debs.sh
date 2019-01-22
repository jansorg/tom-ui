#!/bin/sh
docker stop tom

mkdir -p ubuntu18.10
docker build --rm -t tom .
docker run -d -t -i --rm --name tom -v "$PWD/ubuntu18.10:/dist" tom
docker exec tom bash /root/build-deb.sh

docker stop tom
