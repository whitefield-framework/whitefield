#!/bin/bash

. ./config.inc

export LD_LIBRARY_PATH=$NS3PATH:$BINDIR

make || exit

if [ "$1" == "gdb" ]; then
	shift
	cmdprefix="gdb --args"
fi

mkdir log pcap 2>/dev/null
$cmdprefix $BINDIR/whitefield $*
