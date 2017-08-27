#!/bin/bash

. config.inc

if [ "$AIRLINE_NS3" != "" ]; then #Build NS3
	cd $AIRLINE_NS3
	./waf configure
	if [ $? -ne 0 ]; then
		echo "********* NS3 Compilation failed *********"
		exit 1
	fi
	cd -
fi

#Build Whitefield
make
if [ $? -ne 0 ]; then
	echo "********* Whitefield Compilation failed *********"
	exit 1
fi

