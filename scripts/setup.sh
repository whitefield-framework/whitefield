#!/bin/bash

. config.inc

chk_cmd_present()
{
	pkgname=$2
	[[ "$2" == "" ]] && pkgname=${1}
	echo -en "checking [$pkgname] ... "
	which $1>/dev/null
	[[ $? -ne 0 ]] && echo "Please install [$pkgname] and restart install" && exit 1
	echo -en "done\n"
}

chk_prerequisite()
{
	sudo apt -y install libc6-dev-i386 git make gcc g++ automake m4 libtool graphviz jq python unzip libboost-all-dev
	chk_cmd_present git
	chk_cmd_present make
	chk_cmd_present gcc
	chk_cmd_present python
	chk_cmd_present g++
	chk_cmd_present automake
	chk_cmd_present m4
	chk_cmd_present jq
	chk_cmd_present libtoolize libtool
	chk_cmd_present dot graphviz
}

chk_prerequisite

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

