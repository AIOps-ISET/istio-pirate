#!/bin/sh
set -x
echo "Configuring iptables..."
# write yout iptables rules
iptables -I INPUT -p tcp --src 127.0.0.1 -j ACCEPT
iptables -nL
