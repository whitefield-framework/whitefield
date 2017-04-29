#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh
TMP="/tmp/wf_graphviz_$$"
POSF="/tmp/wf_node_pos_$$"

ip2id() {
	local id="${1/*:/}"
	printf "%d" 0x$id
}

tree_graph() {
	echo "digraph tree_network { rankdir=\"BT\"" > $TMP

	for((i=0;i<${#nodelist[@]};i++)); do
		id=`echo ${nodelist[$i]} | awk '{ print $3 }'`
		parent_ip=`sl_cmd "$id:cmd_def_route"`
		if [ "$parent_ip" != "[NONE]" ]; then
			prnt_id=`ip2id $parent_ip`
			#echo "Node $id, parent $prnt_id"
			echo "$id -> $prnt_id;" >> $TMP
		fi
	done
	echo "}" >> $TMP
	dot -Tpng $TMP > $1
}

position_graph() {

	if [ $(al_cmd "cmd_node_position:$POSF") != "SUCCESS" ]; then
		echo "failed to get node position!";
		return;
	fi

	echo "graph position_network { " > $TMP
	for((i=0;i<${#nodelist[@]};i++)); do
		id=`echo ${nodelist[$i]} | awk '{ print $3 }'`
		parent_ip=`sl_cmd "$id:cmd_def_route"`
		if [ "$parent_ip" != "[NONE]" ]; then
			prnt_id=`ip2id $parent_ip`
			#echo "Node $id, parent $prnt_id"
			echo "$id -- $prnt_id;" >> $TMP
		fi
	done

	#Dump node absolute position
	readarray nodepos < $POSF
	for((i=0;i<${#nodepos[@]};i++)); do
		IFS=' ' read -r -a arr <<< "${nodepos[$i]}"
		echo "${arr[1]} [ pos=\"${arr[3]},${arr[4]}!\"]" >> $TMP
	done

	echo "}" >> $TMP
	neato -Tpng $TMP > $1
}

main() {
	[[ $# -lt 2 ]] && echo "Usage: $0 <tree_outfile> <pos_outfile>" && exit
	hash dot 2>/dev/null || { echo "Need graphviz for network plotting!"; exit 1; }
	wfpid=`wf_get_pid`
	[[ "$wfpid" == "" ]] && echo "Whitefield is stopped" && exit
	get_node_list
	tree_graph $1
	position_graph $2
}

#Processing starts here...
main $*
rm -f $TMP $POSF
