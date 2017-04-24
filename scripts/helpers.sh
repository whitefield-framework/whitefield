#!/bin/bash

[[ "$DIR" == "" ]] && echo "DIR needs to be set in caller script" && exit
. $DIR/../config.inc

WF_PNAME="whitefield"
FORKER_PNAME="wf_forker"
UDP_TOOL=$DIR/../$BINDIR/udp_cmd
function wf_get_pid()
{
	pgrep -u `whoami` -x $WF_PNAME
}

function al_cmd()
{
	$UDP_TOOL localhost $MONITOR_PORT "AL:$1"
}
