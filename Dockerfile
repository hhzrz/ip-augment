FROM redis:4.0.2

RUN apt-get update \
    && apt-get -y install wget g++ make cmake

RUN mkdir -p /redis-modules/

ADD header/ /redis-modules/header/
ADD lib/ /redis-modules/lib/
ADD CMakeLists.txt /redis-modules/
ADD ip_augment.* /redis-modules/
WORKDIR /redis-modules/
RUN cmake . \
    && make
ADD data/ip2region.db /data/
CMD [ "redis-server", "--loadmodule", "/redis-modules/libip_augment.so", "/data/ip2region.db" ]
