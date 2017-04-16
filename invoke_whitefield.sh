#!/bin/bash

. ./config.inc

export LD_LIBRARY_PATH=$NS3PATH:$BINDIR
export FORKER=$BINDIR/wf_forker
export LOGPATH=log

if [ "$1" == "gdb" ]; then
	shift
	cmdprefix="gdb --args"
fi

mkdir $LOGPATH pcap 2>/dev/null
$cmdprefix $BINDIR/whitefield $*
