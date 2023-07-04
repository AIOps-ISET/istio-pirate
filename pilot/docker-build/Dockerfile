FROM debian:bookworm-slim
ADD lib* /usr/local/lib
RUN touch /etc/apt/sources.list \
    && echo 'deb http://mirrors.tuna.tsinghua.edu.cn/debian/ bookworm main contrib non-free non-free-firmware\n \
    deb http://mirrors.tuna.tsinghua.edu.cn/debian/ bookworm-updates main contrib non-free non-free-firmware\n \
    deb http://mirrors.tuna.tsinghua.edu.cn/debian/ bookworm-backports main contrib non-free non-free-firmware\n \
    deb http://mirrors.tuna.tsinghua.edu.cn/debian-security bookworm-security main contrib non-free non-free-firmware' >/etc/apt/sources.list \
    && rm /etc/apt/sources.list.d/debian.sources && apt-get update && apt-get install -y curl openssl \
    && touch /etc/ld.so.conf.d/custom-libs.conf && echo /usr/local/lib >/etc/ld.so.conf.d/custom-libs.conf && chmod +x /usr/local/lib/*
ADD pilot-app /app/pilot-app
ENTRYPOINT /app/pilot-app
