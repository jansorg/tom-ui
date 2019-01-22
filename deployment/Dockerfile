FROM ubuntu:18.10

RUN apt-get update \
    && apt-get install -y -qq --no-install-recommends \
        cmake \
        dpkg-dev \
        file \
        g++ \
        make \
        qt5-default \
        qtbase5-dev \
        qttools5-dev \
        rpm \
        xvfb \
	build-essential \
	git \
	golang \
	vim \
	software-properties-common \
	libqt5svg5-dev

ADD ubuntu18.10 /dist
COPY build-deb.sh /root 

ENTRYPOINT ["tail", "-f", "/dev/null"]
