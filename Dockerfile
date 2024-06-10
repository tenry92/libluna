# build image: docker build -t cpp-cmake-env .
# run container: docker run -it --rm cpp-cmake-env
# mkdir build && cd build && cmake ..

FROM ubuntu:latest

ARG INSTALL_DEVKITPRO=false

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src
COPY scripts /opt/scripts

RUN /opt/scripts/install-dependencies.sh

RUN if [ "$INSTALL_DEVKITPRO" = "true" ]; then \
    /opt/scripts/install-devkitpro.sh; \
    fi

COPY . /usr/src
CMD ["bash"]
