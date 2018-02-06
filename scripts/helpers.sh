#!/bin/bash

[[ "$DIR" == "" ]] && echo "DIR needs to be set in caller script" && exit
. $DIR/../config.inc

BR_ID=0

WF_PNAME="whitefield"
FORKER_PNAME="wf_forker"
UDP_TOOL=$DIR/../$BINDIR/udp_cmd

wf_get_pid()
{
	pgrep -u `whoami` -x $WF_PNAME
}

al_cmd()
{
	$UDP_TOOL localhost $MONITOR_PORT "AL:$1"
}

sl_cmd()
{
	$UDP_TOOL localhost $MONITOR_PORT "SL:$1"
}

get_node_list()
{
	usr=`whoami`
	readarray nodelist < <(ps -h --ppid `pgrep -u $usr -x $FORKER_PNAME` -o "%p %a")
	nodecnt=${#nodelist[@]}
}

get_node_range()
{
    get_node_list
    start_node=0
    end_node=$nodecnt
    [[ "$node_range" == "" ]] && return 0
    [[ "$node_range" == "*" ]] && return 0
    start_node=${node_range/-*/}
    end_node=${node_range/*-/}
}

# Unset nodeid and optionally set node_range before you call this api
get_next_node()
{
    [[ "$nodeid" == "" ]] && get_node_range && nodeid=0 && return 0
    ((nodeid++))
    [[ $nodeid -lt $end_node ]] && return 0
    unset nodeid
    unset start_node
    unset end_node
    unset node_range
    return 1
}

combine_stats()
{
    unset nodeid
    node_range="$2"
    while get_next_node; do
        $1 "$nodeid" | jq "$3"
    done
}

cmd_for_all_nodes()
{
	get_node_list
	for((i=0;i<$nodecnt;i++)); do
		echo "$1 \"$i:$2\""
		$1 "$i:$2"
		echo ;
		echo ;
	done
}

get_ftok_key()
{
	ipc_file="$DIR/.."
	proj_id=0xab
	dev=`stat --format=%d $ipc_file`
	ino=`stat --format=%i $ipc_file`
	ftok_key=$((($ino&0xffff)|(($dev&0xff)<<16)|(($proj_id&0xff)<<24)))
}

rem_msgq()
{
	get_ftok_key
	ftok_key_hex=`printf "0x%x" $ftok_key`
	ipcs -q | grep "$ftok_key_hex" > /dev/null
	if [ $? -eq 0 ]; then
		#delete messageq only if it exists
		ipcrm -Q $ftok_key
	fi
}

stop_whitefield()
{
	wfpid=`wf_get_pid`
	[[ "$wfpid" == "" ]] && echo "Whitefield NOT UP!!" && rem_msgq && return
	kill -2 $wfpid
	rem_msgq
	[[ $? -ne 0 ]] && echo "Problem stopping whitefield [$wfpid]" && return
	echo "Stopped Whitefield"
	sleep 1
	echo ;
}

plot_network_graph() 
{
	[[ "$2" == "" ]] && echo "Usage: plot_network_graph <tree_png_outfile> <position_png_outfile>" && return
	$DIR/network_graph.sh $1 $2
}

quit() {
	exit 0
}
