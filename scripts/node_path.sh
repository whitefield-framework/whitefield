#!/bin/bash

usage()
{
    echo "Usage: $1 up/down [node-range(def=*)]"
    echo "Example: $1 up 0-10"
    exit
}

[[ ! $1 =~ ^up$|^down$ ]] && usage $0

DIR=`dirname $0`
. $DIR/helpers.sh
SHCMD="$DIR/wfshell"

get_upstream_path()
{
    def_rt=`$SHCMD cmd_def_route $1`
    def_rt=${def_rt/*:/}
    [[ "$def_rt" == "[NONE]" ]] && echo -en "$1" && return
    echo -en "$1 -> "
    def_rti=`printf %d 0x$def_rt`
    get_upstream_path "$def_rti"
}

get_next_hop()
{
    TMP_FILE=route_$$.tab
    next_hop_ip=`$SHCMD cmd_route_table $1 $TMP_FILE`
    next_hop_ip=`jq ".route_table.routes[] | select(.prefix == \"$target_ip\") | .next_hop" -r $TMP_FILE`
    rm $TMP_FILE
    [[ "$next_hop_ip" == "" ]] && echo -en "$1" && return
    next_hop=${next_hop_ip/*:/}
    echo -en "`printf %x $1` -> "
    get_next_hop `printf %d 0x$next_hop`
}

get_downstream_path()
{
    targ_nodeid=`printf %x $1`
    target_ip="fd00::302:304:506:$targ_nodeid"
    get_next_hop 0
}

main()
{
    unset nodeid
    node_range="$2"
    while get_next_node; do
        [[ $1 =~ ^up$ ]] && get_upstream_path $nodeid
        [[ $1 =~ ^down$ ]] && get_downstream_path $nodeid
        echo ;
    done
}

#Processing starts here
main $*
