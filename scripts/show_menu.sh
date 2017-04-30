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

function cmd_for_all_nodes()
{
	get_node_list
	for((i=0;i<${#nodelist[@]};i++)); do
		echo "$1 \"$i:$2\""
		$1 "$i:$2"
		echo ;
		echo ;
	done
}

function show_mac_status()
{
	cmd_for_all_nodes al_cmd "cmd_mac_stats"
	al_cmd "cmd_mac_stats"
	echo ;
}

function menu()
{
	wfpid=`wf_get_pid`
	[[ "$wfpid" == "" ]] && echo "Whitefield is stopped" && exit
	if [ "$1" == "" ]; then
		clear
		echo "---[Menu]---"
		echo "1. show mac status for all nodes"
		echo "D. show Default route for all nodes"
		echo "E. show routE table for all nodes"
		echo "C. show iCmp stats for all nodes"
		echo "I. show Ipv6 stats for all nodes"
		echo "R. show Rpl stats for all nodes"
		echo "N. show Nd6 stats for all nodes"
		echo "P. show node Position for all nodes"
		echo "O. show stackline Os for all nodes"
		echo "U. show Udp stats for all nodes"
		echo "T. show Tcp stats for all nodes"
		echo "Q. Quit/stop Whitefield"
		read -p "Press cmd id or ctrl-c to exit:" ans
	else 
		ans="$1"
		shift
	fi
	if [ "$ans" == "1" ]; then
		show_mac_status
	elif [ "$ans" == "p" ] || [ "$ans" == "P" ]; then
		al_cmd "cmd_node_position"
	elif [ "$ans" == "e" ] || [ "$ans" == "E" ]; then
		cmd_for_all_nodes sl_cmd "cmd_route_table"
	elif [ "$ans" == "d" ] || [ "$ans" == "D" ]; then
		cmd_for_all_nodes sl_cmd "cmd_def_route"
	elif [ "$ans" == "o" ] || [ "$ans" == "O" ]; then
		cmd_for_all_nodes sl_cmd "cmd_node_osname"
	elif [ "$ans" == "n" ] || [ "$ans" == "N" ]; then
		cmd_for_all_nodes sl_cmd "cmd_nd6_stats"
	elif [ "$ans" == "u" ] || [ "$ans" == "U" ]; then
		cmd_for_all_nodes sl_cmd "cmd_udp_stats"
	elif [ "$ans" == "t" ] || [ "$ans" == "T" ]; then
		cmd_for_all_nodes sl_cmd "cmd_tcp_stats"
	elif [ "$ans" == "c" ] || [ "$ans" == "C" ]; then
		cmd_for_all_nodes sl_cmd "cmd_icmp_stats"
	elif [ "$ans" == "i" ] || [ "$ans" == "I" ]; then
		cmd_for_all_nodes sl_cmd "cmd_ipv6_stats"
	elif [ "$ans" == "r" ] || [ "$ans" == "R" ]; then
		cmd_for_all_nodes sl_cmd "cmd_rpl_stats"
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
