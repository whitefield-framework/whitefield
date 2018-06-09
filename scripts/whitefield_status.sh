#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh

function msgq_status()
{
	ipcs -q
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

	#msgq_status

	get_node_list
	echo "Node count: $nodecnt"
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
	echo "Press [?] for wfshell, any other key to exit..."
}

#Processing begins here...
main $*
