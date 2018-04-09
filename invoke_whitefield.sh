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
export AIRLINE_ERR=$LOGPATH/airline_error.log
export AIRLINE_LOG=$LOGPATH/airline.log
#export NS_LOG="*=level_warn:LrWpanMac=level_all"

func_childret()
{
	jobs -l | grep "Running" >/dev/null
	exit $?
}

wfpid=$(pgrep -u `whoami` -x whitefield)
[[ $? -eq 0 ]] && echo "Whitefield(pid=$wfpid) is already in execution!" && exit 1

mkdir $LOGPATH pcap 2>/dev/null
if [ "$cmdprefix" == "" ]; then #Regular execution
	trap func_childret SIGCHLD
	set -m
	$BINDIR/whitefield $* 1>$AIRLINE_LOG 2>$AIRLINE_ERR &
	sleep 1
	echo ;
    cat $AIRLINE_ERR
else #GDB execution
	$cmdprefix $BINDIR/whitefield $*
fi
