FROM debian:bullseye-slim as debian-base
RUN apt update
RUN apt install -y iptables

FROM alpine:latest as alpine-base
RUN apk add iptables

FROM locale/iptables:alpine as init-alpine
ADD init.sh /home
RUN chmod a+x /home/init.sh
ENTRYPOINT "/home/init.sh"