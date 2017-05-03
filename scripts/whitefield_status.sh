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

function get_route_list()
{
	rtsize=`sl_cmd "$BR_ID:cmd_rtsize"`
	echo "Route entries on node $BR_ID: $rtsize"
}

error_log()
{
	ERRCNT=5
	ERRFILE="$DIR/../log/airline_error.log"
	[[ ! -f "$ERRFILE" ]] && echo "ERROR FILE NOT FOUND!!\n" && return
	[[ $(stat -c "%s" $ERRFILE) -le 0 ]] && return
	echo "----[Airline last $ERRCNT ERRORS]----"
	tail -$ERRCNT $ERRFILE
	echo "<<check $ERRFILE for details>>"
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

	al_cmd "cmd_mac_stats"
	echo ;
	echo ;

	str=`uptime`
	pcpu=`ps -h -p $wfpid -o "%C"`
	memusage=`pmap $wfpid | tail -1 | awk '{print $2}'`
	echo "System load avg:${str/*:/}"
	echo "Airline CPU:$pcpu%, Memory:$memusage"
	echo ;

	error_log

	echo ;
	echo ;
	echo "[Press CTRL-C to exit, ? for wfshell]..."
}

#Processing begins here...
main $*
