FROM alpine:3.6

RUN apk add --update build-base && \
    mkdir /cc65

COPY . /cc65

RUN cd /cc65 && \
    make && \
    PREFIX=/ make install
