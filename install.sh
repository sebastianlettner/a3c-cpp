#!/bin/sh

mkdir tmp1/                                                                && \
cd tmp1/                                                                   && \
wget https://github.com/zeromq/libzmq/archive/master.zip && \
unzip master.zip                                                          && \
cd libzmq-master && \
mkdir build && \
cd build && \
cmake .. && \
cd .. && \
cd .. && \
cd .. && \
mv tmp1/libzmq-master/build/platform.hpp external/ && \
rm -r tmp1/

