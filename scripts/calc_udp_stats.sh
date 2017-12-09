#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh

BRID=0
MAX_NODE_CNT=10
WFSH="$DIR/wfshell"

get_node_list
while [ 1 ]; do
	rcvd=`$WFSH cmd_udp_stats $BRID | jq .udp_stats.rcvd -r`
	[[ $rcvd -le 0 ]] && echo "no UDP pkt rcvd on BR" && sleep 1 && continue
	tot_sent=0
	for((i=0;i<$nodecnt;i++)); do
		sent=`$WFSH cmd_udp_stats $i | jq .udp_stats.sent -r`
		ret=$?
		[[ $ret -ne 0 ]] && break
		((tot_sent+=sent))
	done
	drop_ratio=`echo "scale=4; $rcvd/$tot_sent" | bc -lq`
	echo "$rcvd/$tot_sent rcvd/sent. Drop ratio=$drop_ratio"
	sleep 1; 
done

