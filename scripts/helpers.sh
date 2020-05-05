#!/bin/bash

[[ "$DIR" == "" ]] && echo "DIR needs to be set in caller script" && exit
. $DIR/../config.inc

BR_ID=0

WF_PNAME="whitefield"
FORKER_PNAME="wf_forker"
UDP_TOOL=$DIR/../$BINDIR/udp_cmd

function elap_time()
{
	wfpid=`wf_get_pid`
	if [ "$wfpid" == "" ]; then
		echo "Whitefield stopped"
		exit
	fi
	et=`ps -p $wfpid -o etimes=`
	wf_elap_times=`echo $et`
	et=`ps -p $wfpid -o etime=`
	wf_elap_time=`echo $et`
}

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
	readarray nodelist < <(ps -h --ppid `pgrep -u $usr -x $FORKER_PNAME` -o "%p %a" | grep -v "defunct")
	readarray dead_nodelist < <(ps -h --ppid `pgrep -u $usr -x $FORKER_PNAME` -o "%p %a" | grep "defunct")
	nodecnt=${#nodelist[@]}
	dead_nodecnt=${#dead_nodelist[@]}
}

get_node_range()
{
    get_node_list
    start_node=0
    end_node=$nodecnt
    [[ "$nodecnt" == "0" ]] && echo "Nodecnt=$nodecnt. May be whitefield has stopped." && exit
    [[ "$node_range" == "" ]] && return 0
    [[ "$node_range" == "*" ]] && return 0
    start_node=${node_range/-*/}
    end_node=${node_range/*-/}
}

# Unset nodeid and optionally set node_range before you call this api
get_next_node()
{
    [[ "$nodeid" == "" ]] && get_node_range && nodeid=$start_node && return 0
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
	wfpid=`wf_get_pid`
	[[ "$wfpid" != "" ]] && echo "Whitefield not stopped cleanly. Terminating forcefully!" && kill -9 $wfpid
	echo ;
}

plot_network_graph() 
{
	[[ "$2" == "" ]] && echo "Usage: plot_network_graph <tree_png_outfile> <position_png_outfile>" && return
    tplot="$1"
    pplot="$2"
    shift 2
    while [ "$1" != "" ]; do
        export $1
        shift
    done
	$DIR/network_graph.sh $tplot $pplot
}

quit() {
	exit 0
}

path_upstream()
{
    $DIR/node_path.sh up $1
}

path_downstream()
{
    $DIR/node_path.sh down $1
}

# Note that OpenThread nodeids start from 1 and above ...
# Whitefield nodeids start from 0 and above ...
native_shell()
{
    [[ "$1" == "" ]] && echo "Usage: native_shell <nodeid>" && return
    udspath=`printf "log/%04x.uds" $1`
    echo -en "connecting to [$udspath]..."
    echo "" | socat UNIX:$udspath -
    [[ $? -ne 0 ]] && return
    socat UNIX:$udspath -
}
