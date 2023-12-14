#!/bin/sh
for i in $(cat $(dirname "$0")/config/ProtectList.txt); do
    [ ! -z "$(echo $i | grep '#')" ] && continue
    pm enable $i
done
