#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh

function msgq_status()
{
	ipcs -q
}

function elap_time()
{
	wfpid=`wf_get_pid`
	if [ "$wfpid" == "" ]; then
		echo "Whitefield stopped"
		exit
	fi
	et=`ps -p $wfpid -o etime=`
	wf_elap_time=`echo $et`
}

function get_node_list()
{
	usr=`whoami`
	readarray nodelist < <(ps -h --ppid `pgrep -u $usr -x $FORKER_PNAME` -o "%p %a")
}

function get_route_list()
{
	echo "TODO: Show route count on BR ... how to identify which node is BR??"
}

function main()
{
	elap_time
	echo "Simulation elapsed time: $wf_elap_time"

	msgq_status

	get_node_list
	echo "Node count: ${#nodelist[@]}"

	get_route_list

	echo ;
	str=`uptime`
	echo "System load avg:${str/*:/}"
	echo ;

	echo "Press ctrl-c to exit, ? for menu..."
}

#Processing begins here...
main $*
