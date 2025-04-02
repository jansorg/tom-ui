FROM ubuntu:22.04

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update \
    && apt-get install -y -qq --no-install-recommends \
        cmake \
        dpkg-dev \
        file \
        g++ \
        make \
        libgl1-mesa-dev \
        qt6-base-dev \
        qt6-base-private-dev \
        qt6-base-dev-tools \
        qt6-webengine-dev \
        qt6-webengine-private-dev \
        qt6-webengine-dev-tools \
        libqt6webenginewidgets6 \
        qt6-webview-dev \
        libqt6opengl6-dev \
        libqt6svgwidgets6 \
        libqt6svg6-dev \
        qt6-tools-dev \
        qt6-tools-dev-tools \
        qt6-l10n-tools \
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
