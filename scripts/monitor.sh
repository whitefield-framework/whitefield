#!/bin/bash

DIR=`dirname $0`
watch -n2 $DIR/whitefield_status.sh &
WATCH_PID=$!
read -n 1 ans
kill $WATCH_PID
if [ "$ans" == "?" ]; then
	$DIR/wfshell
else
	sleep 1
	echo ;
fi
