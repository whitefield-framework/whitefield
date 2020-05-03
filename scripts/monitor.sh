#!/bin/bash

DIR=`dirname $0`
watch -n2 $DIR/whitefield_status.sh $*

# Have commented below after getting problem with "read: read resource temporarily unavailable"
#WATCH_PID=$!
#read -n 1 ans
#kill $WATCH_PID
#if [ "$ans" == "?" ]; then
#	$DIR/wfshell
#else
#	sleep 1
#	echo ;
#fi
