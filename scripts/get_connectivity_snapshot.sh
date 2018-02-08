#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh
CMDLIST="$DIR/internal/cmd_list.txt"
SHCMD="$DIR/wfshell"

get_node_list
declare -a nlist_defrt
declare -a nlist_rttab
declare -a nlist_rtcnt_act  #actual entries present in the route table
declare -a nlist_rtcnt_exp  #expected entries present in the route table
unconn_nodes=0

for((i=0;i<$nodecnt;i++)); do
    def_rt=`$SHCMD cmd_def_route $i`
    def_rt=${def_rt/*:/}
    rt_list=`$SHCMD cmd_route_table $i | jq -r ".route_table.routes[].prefix" | sed -e 's/.*://g'`
    rt_list=`echo $rt_list`
    nodeid=`printf %x $i`

    #echo -en "$nodeid:$def_rt: $rt_list \n"
    nlist_defrt[$i]=$def_rt
    nlist_rttab[$i]=" $rt_list "
    declare -a arr=(${nlist_rttab[$i]})
    nlist_rtcnt_act[$i]=${#arr[@]}
    nlist_rtcnt_exp[$i]=0
done

#1. get next node (target)
#2. get default route entry for that node
#3. check if that default route contains an rtentry for the target... if yes, increment rt_cnt_act, if no, return
#4. set default route as the next node and goto step 2
verify_rtentry()
{
    [[ "$1" == "0" ]] && return
    nodeid=`printf %x $1`
    def_rt=${nlist_defrt[$1]}

    #except BR (which is handled above) we do not expect any other node to have def rt as NONE
    [[ "$def_rt" == "[NONE]" ]] && disconnected=1 && return
    def_rtx=${def_rt/*:/}           #default route nodeid in hex
    def_rti=`printf %d 0x$def_rtx`  #default route nodeid in int

    #echo "$def_rtx: ${nlist_rttab[$def_rti]} == [ $targ_nodeid ]"
    echo "${nlist_rttab[$def_rti]}" | grep " $targ_nodeid " >/dev/null
    if [ $? -ne 0 ]; then
        disconnected=1
        return
    else
        ((nlist_rtcnt_exp[$def_rti]++))
    fi
    verify_rtentry $def_rti
}

# Check unconnected nodes and get expected route table count
for((i=0;i<$nodecnt;i++)); do
    disconnected=0
    targ_nodeid=`printf %x $i`
    #echo "Tracing $targ_nodeid ..."
    verify_rtentry $i
    if [ $disconnected -ne 0 ]; then
        ((unconn_nodes++))
    fi
done

tot_stale_cnt=0
tot_6ln=0
for((i=0;i<$nodecnt;i++)); do
    #echo "$i: exp=${nlist_rtcnt_exp[$i]} act=${nlist_rtcnt_act[$i]} rt=${nlist_rttab[$i]}"
    [[ ${nlist_rtcnt_exp[$i]} -eq 0 ]] && ((tot_6ln++))
    ((stale_cnt=${nlist_rtcnt_act[$i]} - ${nlist_rtcnt_exp[$i]}))
    [[ $stale_cnt -lt 0 ]] && stale_cnt=0
    ((tot_stale_cnt+=$stale_cnt))
done

echo "Total Nodes: $nodecnt"
echo "Leaf Nodes: $tot_6ln"
echo "Unconnected nodes: $unconn_nodes"
echo "Stale Routing Entries: $tot_stale_cnt"
#Show graph based on downstream connectivity ... with DCO this graph should look better

