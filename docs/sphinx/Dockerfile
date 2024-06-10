FROM python:3.10-slim

WORKDIR /docs

RUN apt-get update && \
    apt-get install -y  wget && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN pip3 install sphinx sphinx_rtd_theme

RUN sphinx-build --version
