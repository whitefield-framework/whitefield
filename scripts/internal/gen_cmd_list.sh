#!/bin/bash

echo "THIS SCRIPT SHOULD NO MORE BE USED."
exit

DIR="`dirname $0`/.."
. $DIR/helpers.sh
CMDLIST="`dirname $0`/cmd_list.txt"
BASE="$DIR/.."

SL_CMD_C=$BASE/src/stackline/sl_command.c
AL_CMD_C=$BASE/src/airline/Command.cc

gen_cmd() {
	cmds=`grep "HANDLE_CMD" $2 | sed 's/.*cmd_\(.*\))/cmd_\1/g'`
	for c in $cmds; do
		echo "$1 $c"
	done
}

gen_cmd "sl" $SL_CMD_C > $CMDLIST
gen_cmd "al" $AL_CMD_C >> $CMDLIST

#Framework commands needs to be given here manually
echo "fw stop_whitefield" >> $CMDLIST
echo "fw plot_network_graph" >> $CMDLIST
echo "fw help" >> $CMDLIST
echo "fw exit" >> $CMDLIST
