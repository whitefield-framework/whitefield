#!/bin/bash

[[ "`whereis realpath | cut -d ':' -f 2`" == "" ]] && echo "realpath cmd not found. Install coreutils." && exit
[[ ! -f "$1" ]] && [[ ! -d "$1" ]] && echo "Usage: $0 <testcfg || testdir>" && exit

DIR=`dirname "$0"`
REG_DIR=`realpath "$DIR"`
WF_CMD="$REG_DIR/../invoke_whitefield.sh"
WFSH="$REG_DIR/../scripts/wfshell"
TC_LOG="$REG_DIR/testcase_report.log"
rm $TC_LOG 2>/dev/null
tc_cnt=0
tc_fail=0

get_status_print()
{
	RED='\033[0;31m'
	GREEN='\033[0;32m'
	NC='\033[0m' # No Color
	[[ $1 -eq 0 ]] && tc_status="${GREEN}PASS${NC}" && return
	tc_status="${RED}FAIL${NC}"
}

pre_tc_exec()
{
	$WFSH stop_whitefield >/dev/null
	unset TC_MSG
}

post_tc_exec()
{
	$WFSH stop_whitefield >/dev/null
	unset TC_MSG
}

tc_set_msg()
{
	TC_MSG="$*"
}

exec_testcase()
{
	TC_DIR=`dirname "$1"`
	tcname=`basename "$1"`
	echo "executing $1..."
	echo "-----[TEST: $1]-----" >> $TC_LOG

	#load testcase
	source "$1"

	#execute testcase
	pre_tc_exec
	testcase >>$TC_LOG
	post_tc_exec

	#print testcase status
	tc_res=$?
	((tc_cnt++))
	[[ $tc_res -ne 0 ]] && ((tc_fail++))
	get_status_print $tc_res
	echo -en "[$tc_cnt] [$tcname] [$tc_status] [tot_fail:$tc_fail]\n" | tee -a $TC_LOG
	[[ "$TC_MSG" != "" ]] && echo -en "tc_msg:$TC_MSG\n"
}

regress_dir()
{
	tclist=`find "$1" -name "TC_*.test" -type f`
	for tc in $tclist; do
		exec_testcase "$tc"
	done
}

regress_cfg()
{
	[[ -d "$1" ]] && regress_dir "$1" && return
	while read line; do 
		line=`echo $line`	#trim spaces
		[[ "${line}" == "" ]] && continue;
		[[ "${line:0:1}" == "#" ]] && continue;
		path="$REG_DIR/$line"
		if [ -d "$path" ]; then
			regress_dir "$path"
		elif [ -f "$path" ]; then 
			exec_testcase "$path"
		else
			echo "Invalid line:<$line> in <$1> ... ignoring..."
		fi
	done < "$1"
}

regress_cfg "$1"
echo "check $TC_LOG for execution log..."
[[ $tc_fail -ne 0 ]] && echo "$tc_fail testcases failed!!" && exit 1
exit 0
