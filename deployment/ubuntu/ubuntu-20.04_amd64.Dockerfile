FROM ubuntu:20.04

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update \
    && apt-get install -y -qq --no-install-recommends \
        cmake \
        dpkg-dev \
        file \
        g++ \
        make \
        qt5-default \
        qtbase5-dev \
        qtbase5-private-dev \
        libqt5webview5-dev \
        qtwebengine5-dev \
        qttools5-dev \
        rpm \
        xvfb \
	build-essential \
	git \
	golang \
	vim \
	software-properties-common \
	libqt5svg5-dev && \
	dpkg-reconfigure --frontend noninteractive tzdata

COPY build-ubuntu-deb.sh /root

ENTRYPOINT ["tail", "-f", "/dev/null"]
