FROM ubuntu:24.04

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update \
    && apt-get install -y -qq --no-install-recommends \
        cmake \
        dpkg-dev \
        file \
        g++ \
        make \
        qt6-base-dev \
        qt6-base-private-dev \
        qt6-webview-dev \
        qt6-webengine-dev \
        qt6-tools-dev \
        rpm \
        xvfb \
	build-essential \
	git \
	golang \
	vim \
	software-properties-common \
	libqt6svg6-dev \
	tzdata && \
	dpkg-reconfigure --frontend noninteractive tzdata

COPY build-ubuntu-deb.sh /root

ENTRYPOINT ["tail", "-f", "/dev/null"]
