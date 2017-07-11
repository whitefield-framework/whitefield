#!/bin/bash

. config.inc

if [ "$AIRLINK" == "NS3" ]; then #Build NS3
	cd `dirname $NS3PATH`
	./waf configure && make
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

#Build Contiki examples
cd thirdparty/contiki
make -C examples/ipv6/rpl-udp TARGET=whitefield
if [ $? -ne 0 ]; then
	echo "********* Contiki Compilation failed *********"
	exit 1
fi
cd -

