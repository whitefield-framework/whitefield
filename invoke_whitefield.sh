#!/bin/bash

if [ "$1" == "gdb" ]; then
	shift
	cmdprefix="gdb --args"
fi

[[ ! -f "$1" ]] && echo "Usage: $0 <wf_config_file>" && exit
cfgfile=`pwd`"/$1"

cd `dirname $0`
[[ ! -f "config.inc" ]] && echo "Need to start whitefield from base folder!!" && exit 1
. config.inc

export LD_LIBRARY_PATH=$AIRLINE_NS3/build:$BINDIR
export FORKER=$BINDIR/wf_forker
export LOGPATH=log
export MONITOR_PORT=$MONITOR_PORT

func_childret()
{
	jobs -l | grep "Running" >/dev/null
	exit $?
}

mkdir $LOGPATH pcap 2>/dev/null
if [ "$cmdprefix" == "" ]; then
	#Regular execution
	trap func_childret SIGCHLD
	set -m
	$cmdprefix $BINDIR/whitefield $* &
	sleep 1
	echo ;
else
	#GDB execution
	$cmdprefix $BINDIR/whitefield $*
fi
