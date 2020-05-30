#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh
. $DIR/get_node_attr.sh
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
        echo "$id -> 0[ style = invis ];" >> $TMP
		if [[ $parent_ip == [fF][eE]80::* ]]; then
			prnt_id=`ip2id $parent_ip`
			#echo "Node $id, parent $prnt_id"
			echo "$id -> $prnt_id;" >> $TMP
		fi
	done
	for((i=0;i<${#nodelist[@]};i++)); do
        color=`get_node_color $i | jq -r .color`
		echo "$i [ fillcolor=$color style=filled]" >> $TMP
	done
	echo "}" >> $TMP
	dot -Tpng $TMP > $1
	echo "Dumped tree based network graph to file $1"
}

position_graph() {

	if [ $(al_cmd "cmd_node_position:$POSF") != "SUCCESS" ]; then
		echo "failed to get node position!";
		return;
	fi

	echo "digraph position_network { " > $TMP
	for((i=0;i<${#nodelist[@]};i++)); do
		id=`echo ${nodelist[$i]} | awk '{ print $3 }'`
		parent_ip=`sl_cmd "$id:cmd_def_route"`
        echo "$id -> 0[ style = invis ];" >> $TMP
		if [[ $parent_ip == [fF][eE]80::* ]]; then
			prnt_id=`ip2id $parent_ip`
			#echo "Node $id, parent $prnt_id"
			echo "$id -> $prnt_id;" >> $TMP
		fi
	done

	#Dump node absolute position
	#cat $POSF
	readarray nodepos < $POSF
    [[ "$scale" == "" ]] && scale=1
	for((i=0;i<${#nodepos[@]};i++)); do
        color=`get_node_color $i | jq -r .color`
		IFS=' ' read -r -a arr <<< "${nodepos[$i]}"
        xpos=`echo "${arr[3]}*$scale" | bc -q`
        ypos=`echo "${arr[4]}*$scale" | bc -q`
		echo "${arr[1]} [ pos=\"${xpos},${ypos}!\" fillcolor=$color style=filled]" >> $TMP
	done

	echo "}" >> $TMP
	neato -n -Tpng $TMP -o $1
	echo "Dumped position based network graph to file $1"
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
