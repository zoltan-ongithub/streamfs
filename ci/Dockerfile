FROM ubuntu:18.04
ARG HOSTUID
ARG HOSTGID

RUN apt-get update
RUN apt-get install -y libfuse-dev libboost-thread-dev cmake libboost-filesystem-dev \
        build-essential libgtest-dev libgoogle-glog-dev curl 
RUN apt-get install -y libcurl4-openssl-dev

RUN cd /usr/src/gtest; cmake CMakeLists.txt; make ; make install
