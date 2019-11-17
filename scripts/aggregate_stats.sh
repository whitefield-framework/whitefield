#!/bin/bash

usage()
{
    echo "Usage: $1 <cmd> <field> [node-range(def=*)]"
    echo "Example: $1 cmd_rpl_stats .rpl_stats.parent_switch 0-10"
    exit 2
}

[[ "$2" == "" ]] && usage $0

hash jq 2>/dev/null
[[ $? -ne 0 ]] && echo "Install jq (json parser)." && exit 1

DIR=`dirname $0`
. $DIR/helpers.sh
SHCMD="$DIR/wfshell"

unset nodeid
node_range="$3"
cnt=0
valstr="0"
while get_next_node; do
    cmd="$SHCMD $1 $nodeid | jq -r \"$2\""
    val=`$SHCMD $1 $nodeid | jq -r "$2"`
    [[ $? -ne 0 ]] && echo -en "failed cmd: $cmd" && exit 1
    valstr="$valstr + $val"
    ((cnt++))
done
tot=`echo "$valstr" | bc -q`
avg=`echo "scale=2; $tot/$cnt" | bc -q`
echo "{ \"total\": \"$tot\",  \"average\": \"$avg\" }"
