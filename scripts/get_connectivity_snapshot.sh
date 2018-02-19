#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh
CMDLIST="$DIR/internal/cmd_list.txt"
SHCMD="$DIR/wfshell"
AGGCMD="$DIR/aggregate_stats.sh"

usage()
{
    echo "$0 [-e start_after_elapsed_time] [-i interval_to_report] [-c sample_cnt] [-o output_file]"
    exit
}

wr_report()
{
    [[ "$g_output_file" != "" ]] && echo "$*" >> $g_output_file
    echo "$*"
}

wait_sec()
{
    [[ "$1" == "" ]] && return
    cnt=$1
    while [ $cnt -gt 0 ]; do
        echo -en "sleeping ${cnt}s...\r"
        sleep 1
        ((cnt--))
    done
    echo -en "\033[2K"
}

cmdline_args() #XXX Currently unused but can be put to use in future.
{
    elap_time   #exits if whitefield is not up
    g_elap_time=0
    g_interval=5
    g_sample_cnt=1
	while getopts "e:i:c:o:" o; do
		case "$o" in
			e)
				g_elap_time=$OPTARG
                [[ $wf_elap_times -gt $g_elap_time ]] && echo "Whitefield elapsed time is already lapsed." && exit 
				;;
			i)
				g_interval=$OPTARG
				;;
			o)
				g_output_file=$OPTARG
                rm -f $g_output_file
				;;
			c)
				g_sample_cnt=$OPTARG
				;;
			*) 
				usage
				;;
		esac
	done
    shift $((OPTIND-1))
    [[ "$*" != "" ]] && usage
    if [ $g_elap_time -gt 0 ]; then
        ((slp_time=$g_elap_time-$wf_elap_times))
        wait_sec $slp_time
    fi
}

agg_stats()
{
    nr=$noderange
    unset noderange
    str=`$AGGCMD $1 $2 $nr`
    tot=${str/ */}
    eval $3=${tot/total=/}
    if [ "$4" != "" ]; then
        avg=${str/* /}
        eval $4=${avg/avg=/}
    fi
}

get_routing_state_snapshot()
{
    unset nlist_defrt
    unset nlist_rttab
    unset nlist_rtcnt_exp
    unset nlist_rtcnt_act
    declare -g nlist_defrt
    declare -g nlist_rttab
    declare -g nlist_rtcnt_act  #actual entries present in the route table
    declare -g nlist_rtcnt_exp  #expected entries present in the route table

    get_node_list
    unconn_nodes=0
    elap_time   #exits if whitefield is not up
    TMP_FILE="rttable.txt"

    for((i=0;i<$nodecnt;i++)); do
        def_rt=`$SHCMD cmd_def_route $i`
        def_rt=${def_rt/*:/}
        $SHCMD cmd_route_table $i $TMP_FILE >/dev/null
        rt_list=`cat $TMP_FILE | jq -r ".route_table.routes[].prefix" | sed -e 's/.*://g'`
        rm -f $TMP_FILE
        rt_list=`echo $rt_list`
        nodeid=`printf %x $i`

        #echo -en "$nodeid:$def_rt: $rt_list \n"
        nlist_defrt[$i]=$def_rt
        nlist_rttab[$i]=" $rt_list "
        declare -a arr=(${nlist_rttab[$i]})
        nlist_rtcnt_act[$i]=${#arr[@]}
        nlist_rtcnt_exp[$i]=0
    done
    agg_stats cmd_rpl_stats .rpl_stats.parent_switch tot_parent_sw
    agg_stats cmd_rpl_stats .rpl_stats.dao_sent tot_dao_sent
    agg_stats cmd_rpl_stats .rpl_stats.dao_rcvd tot_dao_rcvd
    agg_stats cmd_rpl_stats .rpl_stats.npdao_sent tot_npdao_sent
    agg_stats cmd_rpl_stats .rpl_stats.npdao_rcvd tot_npdao_rcvd
    agg_stats cmd_rpl_stats .rpl_stats.dco_sent tot_dco_sent
    agg_stats cmd_rpl_stats .rpl_stats.dco_rcvd tot_dco_rcvd
    noderange=0
    agg_stats cmd_udp_stats .udp_stats.rcvd tot_udp_rcvd
    agg_stats cmd_udp_stats .udp_stats.sent tot_udp_sent
}

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

verify_routing_state()
{
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
}

get_stats()
{
    tot_stale_cnt=0
    tot_6ln=0
    for((i=0;i<$nodecnt;i++)); do
        #echo "$i: exp=${nlist_rtcnt_exp[$i]} act=${nlist_rtcnt_act[$i]} rt=${nlist_rttab[$i]}"
        [[ ${nlist_rtcnt_exp[$i]} -eq 0 ]] && ((tot_6ln++))
        ((stale_cnt=${nlist_rtcnt_act[$i]} - ${nlist_rtcnt_exp[$i]}))
        [[ $stale_cnt -lt 0 ]] && stale_cnt=0
        ((tot_stale_cnt+=$stale_cnt))
    done

    wr_report "$s,$nodecnt,$tot_6ln,$unconn_nodes,$tot_stale_cnt,$tot_parent_sw,$wf_elap_times,$tot_dao_sent,$tot_dao_rcvd,$tot_npdao_sent,$tot_npdao_rcvd,$tot_dco_sent,$tot_dco_rcvd,$tot_udp_sent,$tot_udp_rcvd"
}

main_loop()
{
    wr_report "num,tot_nodes,lf_nodes,unconn_nodes,stale_entries,tot_par_sw,elap_time,dao_sent,dao_rcvd,npdao_sent,npdao_rcvd,dco_sent,dco_rcvd,udp_sent,udp_rcvd"
    for((s=1;s<=$g_sample_cnt;s++)); do
        [[ $s -gt 1 ]] && wait_sec $g_interval
        get_routing_state_snapshot
        verify_routing_state
        get_stats
    done
}

#Processing starts here main()
cmdline_args $*
shift $((OPTIND-1))
main_loop

#Show graph based on downstream connectivity ... with DCO this graph should look better
