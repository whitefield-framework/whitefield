#!/bin/bash

[[ "$DIR" == "" ]] && echo "DIR needs to be set in caller script" && exit
. $DIR/../config.inc

BR_ID=0

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

function sl_cmd()
{
	$UDP_TOOL localhost $MONITOR_PORT "SL:$1"
}

function get_node_list()
{
	usr=`whoami`
	readarray nodelist < <(ps -h --ppid `pgrep -u $usr -x $FORKER_PNAME` -o "%p %a")
}

function cmd_for_all_nodes()
{
	get_node_list
	for((i=0;i<${#nodelist[@]};i++)); do
		echo "$1 \"$i:$2\""
		$1 "$i:$2"
		echo ;
		echo ;
	done
}

function stop_whitefield()
{
	wfpid=`wf_get_pid`
	[[ "$wfpid" == "" ]] && echo "Whitefield NOT UP!!" && return
	kill -2 $wfpid
	[[ $? -ne 0 ]] && echo "Problem stopping whitefield [$wfpid]" && return
	echo "Stopped Whitefield"
	sleep 1
	echo ;
}

function plot_network_graph() 
{
	echo "TODO"
}
