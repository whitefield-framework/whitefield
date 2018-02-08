#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh
CMDLIST="$DIR/internal/cmd_list.txt"
SHCMD="$DIR/wfshell"

get_node_list
echo "Total Nodes: $nodecnt"
declare -a nlist_defrt
declare -a nlist_rttab
declare -a nlist_rtcnt_act
unconn_nodes=0

for((i=0;i<$nodecnt;i++)); do
    def_rt=`$SHCMD cmd_def_route $i`
    def_rt=${def_rt/*:/}
    rt_list=`$SHCMD cmd_route_table $i | jq -r ".route_table.routes[].prefix" | sed -e 's/.*://g'`
    rt_list=`echo $rt_list`
    nodeid=`printf %x $i`
    echo -en "$nodeid:$def_rt: $rt_list \n"
    nlist_defrt[$i]=$def_rt
    nlist_rttab[$i]=" $rt_list "
done

#1. get next node (target)
#2. get default route entry for that node
#3. check if that default route contains an rtentry for the target... if yes, increment rt_cnt_act
verify_rtentry()
{
    [[ "$1" == "0" ]] && return
    nodeid=`printf %x $1`
    def_rt=`$SHCMD cmd_def_route $1`
    [[ "$def_rt" == "[NONE]" ]] && ((unconn_nodes++)) && return
    def_rtx=${def_rt/*:/}           #default route nodeid in hex
    def_rti=`printf %d 0x$def_rtx`  #default route nodeid in int

    echo "$def_rtx: ${nlist_rttab[$def_rti]} == [ $targ_nodeid ]"
    [[ "${nlist_rttab[$def_rti]}" != *" $targ_nodeid "* ]] && ((unconn_nodes++)) && return
    ((nlist_rtcnt_exp[$1]++))
    verify_rtentry $def_rti
}

for((i=0;i<$nodecnt;i++)); do
    echo "Tracing $i ..."
    targ_nodeid=`printf %x 0x$i`
    verify_rtentry $i
    [[ $unconn_nodes -gt 0 ]] && exit
done

echo "Unconnected nodes: $unconn_nodes"
#Leaf nodes
#Show graph based on downstream connectivity ... with DCO this graph should look better

