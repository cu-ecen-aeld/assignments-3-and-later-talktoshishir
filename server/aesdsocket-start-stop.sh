#!/bin/sh

if [ "$1" = "start" ]; then
    echo "starting aesdsocket server!"
    start-stop-daemon -S -n aesdsocket -a /usr/bin/aesdsocket -- -d
elif [ "$1" = "stop" ]; then
    echo "stopping aesdsocket server!"
    start-stop-daemon -K -n aesdsocket --signal TERM
else
    echo "Usage: $0 command <start|stop>"
    exit 1
fi
