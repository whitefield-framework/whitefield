#!/bin/bash

[[ "`whereis jq | cut -d ':' -f 2`" == "" ]] && echo "jq cmd not found. Install jq (JSON parser)." && exit
[[ ! -f "$1" ]] && [[ ! -d "$1" ]] && echo "Usage: $0 <testcfg || testdir>" && exit

DIR=`dirname "$0"`
REG_DIR=`readlink -f "$DIR"`
WF_CMD="$REG_DIR/../invoke_whitefield.sh"
WFSH="$REG_DIR/../scripts/wfshell"
TC_LOG="$REG_DIR/testcase_report.log"
rm $TC_LOG 2>/dev/null
tc_cnt=0
tc_fail=0

echoscr()
{
	>&2 echo $*;
}

wait4sec()
{
	[[ $# -ne 3 ]] && echo "Usage: $0 <waittime> <cmd> <expoutput>" && return 1
	for((i=0;i<=$1;i++)); do
		echoscr -en "\rwaiting $i/$1 sec..."
		out=`$2`
		[[ "$out" == "$3" ]] && tc_set_msg "reached in $i sec" && ret=0 && break
		sleep 1
	done
	echoscr ;
	[[ $ret -ne 0 ]] && tc_set_msg "($2) expected output ($3) never reached in $1 sec time"
	return $ret
}

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
	tcpath=`readlink -f "$1"`
	TC_DIR=`dirname "$tcpath"`
	tcname=`basename "$1"`
	echo "executing $1..."
	echo "-----[TEST: $1]-----" >> $TC_LOG

	#load testcase
	source "$1"

	#execute testcase
	pre_tc_exec
	testcase >>$TC_LOG

	#print testcase status
	tc_res=$?
	((tc_cnt++))
	[[ $tc_res -ne 0 ]] && ((tc_fail++))
	get_status_print $tc_res
	echo -en "[$tc_cnt] [$tcname] [$tc_status] [tot_fail:$tc_fail]\n" | tee -a $TC_LOG
	[[ "$TC_MSG" != "" ]] && echo -en "tc_msg:$TC_MSG\n"
	post_tc_exec
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
	[[ $1 =~ .*TC_.*\.test$ ]] && exec_testcase "$1" && return
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
[[ $tc_fail -ne 0 ]] && echo "$tc_fail/$tc_cnt testcases failed!!" && exit 1
echo "$tc_cnt testcases passed"
exit 0
