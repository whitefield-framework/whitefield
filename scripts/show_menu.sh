#!/bin/bash

# Usage:
# 1. show_menu.sh -> shows menu and you can manually select the menu option
# 2. show_menu.sh <cmd_id> [arg1]..[argn] -> execute operation specified by cmd_id+args without showing menu options

DIR=`dirname $0`
. $DIR/helpers.sh

function stop_whitefield()
{
	wfpid=`wf_get_pid`
	[[ "$wfpid" == "" ]] && echo "Whitefield NOT UP!!" && return
	kill -2 $wfpid
	[[ $? -ne 0 ]] && echo "Problem stopping whitefield [$wfpid]" && return
	echo "Stopped Whitefield"
	sleep 1
	echo ;
}

function show_mac_status()
{
	get_node_list
	for((i=0;i<${#nodelist[@]};i++)); do
		echo "Node $i:"
		al_cmd "$i:cmd_mac_stats"
		echo ;
		echo ;
	done
	al_cmd "cmd_mac_stats"
}

function menu()
{
	if [ "$1" == "" ]; then
		clear
		echo "---[Menu]---"
		echo "1. show mac status for all nodes"
		echo "2. show all stats for node"
		echo "S. Stop a node (radio turn off)"
		echo "T. sTop a node (turn on radio)"
		echo "Q. Quit/stop Whitefield"
		read -p "Press cmd id or ctrl-c to exit:" ans
	else 
		ans="$1"
		shift
	fi
	if [ "$ans" == "1" ]; then
		show_mac_status
		echo ;
	elif [ "$ans" == "s" ] || [ "$ans" == "S" ]; then
		echo "TODO"
	elif [ "$ans" == "q" ] || [ "$ans" == "Q" ]; then
		stop_whitefield
	else
		echo "Unknown cmd [$ans]" 
	fi
	exit
}

#Processing starts here
menu $*
