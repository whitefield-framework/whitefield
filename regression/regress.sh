#!/bin/bash

[[ "`whereis realpath | cut -d ':' -f 2`" == "" ]] && echo "realpath cmd not found. Install coreutils." && exit
[[ ! -f "$1" ]] && [[ ! -d "$1" ]] && echo "Usage: $0 <testcfg || testdir>" && exit

DIR=`dirname "$0"`
REG_DIR=`realpath "$DIR"`
WF_CMD="$REG_DIR/../invoke_whitefield.sh"
TC_LOG="$REG_DIR/testcase_report.log"
rm $TC_LOG 2>/dev/null
tc_cnt=0
tc_fail=0

exec_testcase()
{
	TC_DIR=`dirname "$1"`
	tcname=`basename "$1"`
	echo "executing $1..."
	echo "-----[TEST: $1]-----" >> $TC_LOG

	#load testcase
	source "$1"

	#execute testcase
	testcase >>$TC_LOG

	#print testcase status
	tc_res=$?
	((tc_cnt++))
	[[ $tc_res -ne 0 ]] && ((tc_fail++))
	echo -en "$tc_cnt $tcname res:$tc_res tot_fail:$tc_fail\n" | tee -a $TC_LOG
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
		[[ "${line:0:1}" == "#" ]] && continue;
		line="$REG_DIR/$line"
		[[ -d "$line" ]] && regress_dir "$line" && continue
		[[ -f "$line" ]] && exec_testcase "$line" && continue
		echo "Invalid line:<$line> in <$1> ... ignoring..."
	done < "$1"
}

regress_cfg "$1"
echo "check $TC_LOG for execution log..."
[[ $tc_fail -ne 0 ]] && exit 1
