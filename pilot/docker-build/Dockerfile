ARG ISTIO_DIR=/home/istio-pirate
ARG LINK_DIR=/usr/local/lib

FROM debian:bookworm-slim as pilot-dev-base
RUN touch /etc/apt/sources.list \
    && echo 'deb http://mirrors.tuna.tsinghua.edu.cn/debian/ bookworm main contrib non-free non-free-firmware\n \
    deb http://mirrors.tuna.tsinghua.edu.cn/debian/ bookworm-updates main contrib non-free non-free-firmware\n \
    deb http://mirrors.tuna.tsinghua.edu.cn/debian/ bookworm-backports main contrib non-free non-free-firmware\n \
    deb http://mirrors.tuna.tsinghua.edu.cn/debian-security bookworm-security main contrib non-free non-free-firmware' >/etc/apt/sources.list \
    && rm /etc/apt/sources.list.d/debian.sources && apt-get update && apt-get install -y cmake git build-essential libssl-dev libcurl4-openssl-dev zlib1g-dev uncrustify \
    && mkdir /home/temp && cd /home/temp \
    && git clone https://libwebsockets.org/repo/libwebsockets --depth 1 --branch v4.2-stable \
    && cd libwebsockets && mkdir build && cd build \
    && cmake -DLWS_WITHOUT_TESTAPPS=ON -DLWS_WITHOUT_TEST_SERVER=ON -DLWS_WITHOUT_TEST_SERVER_EXTPOLL=ON \
          -DLWS_WITHOUT_TEST_PING=ON -DLWS_WITHOUT_TEST_CLIENT=ON -DCMAKE_C_FLAGS="-fpic" -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && make && make install \
    && cd /home/temp/ && git clone https://github.com/yaml/libyaml --depth 1 --branch release/0.2.5 && cd libyaml \
    && mkdir build && cd build \
    && cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_TESTING=OFF  -DBUILD_SHARED_LIBS=ON .. && make && make install \
    && cd /home/temp && git clone https://github.com/kubernetes-client/c && CLIENT_REPO_ROOT=${PWD}/c \
    && cd ${CLIENT_REPO_ROOT}/kubernetes && mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make && make install \
    && rm -rf /home/temp
ENTRYPOINT /bin/bash

FROM local/pilot-dev-base:latest as build-pilot
ARG ISTIO_DIR
ARG LINK_DIR
COPY istio-pirate $ISTIO_DIR
RUN cd $ISTIO_DIR && mkdir build && cd build && cmake .. && make
ENTRYPOINT /bin/bash

FROM debian:bookworm-slim as pilot-runtime-base
ARG LINK_DIR
COPY --from=build-pilot $LINK_DIR/libwebsockets.so.18 $LINK_DIR/libyaml.so $LINK_DIR/libkubernetes.so \
    /lib/x86_64-linux-gnu/libcrypto.so.3 /lib/x86_64-linux-gnu/libcurl.so.4 /lib/x86_64-linux-gnu/libssl.so.3 \
    /lib/x86_64-linux-gnu/libnghttp2.so.14 /lib/x86_64-linux-gnu/librtmp.so.1 /lib/x86_64-linux-gnu/libssh2.so.1 \
    /lib/x86_64-linux-gnu/libpsl.so.5 /lib/x86_64-linux-gnu/libgssapi_krb5.so.2 /lib/x86_64-linux-gnu/libldap-2.5.so.0 \
    /lib/x86_64-linux-gnu/liblber-2.5.so.0 /lib/x86_64-linux-gnu/libbrotlidec.so.1 /lib/x86_64-linux-gnu/libkrb5.so.3 \
    /lib/x86_64-linux-gnu/libk5crypto.so.3 /lib/x86_64-linux-gnu/libkrb5support.so.0 /lib/x86_64-linux-gnu/libsasl2.so.2 \
    /lib/x86_64-linux-gnu/libbrotlicommon.so.1 /lib/x86_64-linux-gnu/libkeyutils.so.1 \
    $LINK_DIR/
RUN touch /etc/ld.so.conf.d/custom-libs.conf && echo /usr/local/lib >/etc/ld.so.conf.d/custom-libs.conf && chmod +x $LINK_DIR/* && /usr/sbin/ldconfig
ENTRYPOINT /bin/bash

FROM local/pilot-runtime-base:latest as pilot
ARG ISTIO_DIR
ENV ISTIO_DIR=$ISTIO_DIR
COPY --from=build-pilot $ISTIO_DIR/build/pilot_app $ISTIO_DIR/
ENTRYPOINT ${ISTIO_DIR}/pilot_app
