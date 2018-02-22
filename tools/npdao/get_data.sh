#!/bin/bash

[[ "$2" == "" || ! -f "$1" ]] && echo "Usage: $0 <cfg_file> <label>" && exit
DIR=`dirname $0`
DIR=`realpath $DIR`
BASE=$DIR/../..
SHCMD="$BASE/scripts/wfshell"
INVOKE_WF="$BASE/invoke_whitefield.sh"
WAIT_ELAP_TIME=60
SAMPLE_INTERVAL=20
SAMPLE_COUNT=60
NUM_OF_RUNS=3
CFG_FILE=`realpath $1`
LABEL="$2"

get_estimated_time()
{
    SECONDS=0
    est_time_sec=`echo "(($SAMPLE_INTERVAL*$SAMPLE_COUNT)+$WAIT_ELAP_TIME)*$NUM_OF_RUNS*2" | bc -q`
    est_time_hms=`printf '%02d:%02d:%02d' $(($est_time_sec/3600)) $(($est_time_sec%3600/60)) $(($est_time_sec%60))`
}

start_wf()
{
    $SHCMD stop_whitefield
    $INVOKE_WF $CFG_FILE
}

take_data()
{
    [[ "$1" == "" ]] && echo "take_data requires label as input" && exit
    for((i=0;i<$NUM_OF_RUNS;i++)); do
        start_wf
        prn_time_info
        $BASE/scripts/get_connectivity_snapshot.sh -e $WAIT_ELAP_TIME -i $SAMPLE_INTERVAL -c $SAMPLE_COUNT -o $DIR/$LABEL/$1$i.csv
    done
}

set_dco_conf()
{
    [[ ! "$1" =~ ^0$|^1$ ]] && echo "Need to pass with 0 or 1" && exit
    TMP_FILE=/tmp/prj_conf_$$.h
    PROJECT_CONF=$BASE/thirdparty/contiki/examples/ipv6/rpl-udp/project-conf.h
    sed -e "s/^#.*define.*RPL_CONF_WITH_DCO.*$/#define RPL_CONF_WITH_DCO $1/g" $PROJECT_CONF > $TMP_FILE
    cp $TMP_FILE $PROJECT_CONF
    rm $TMP_FILE
    make -C $BASE contiki_clean
    make -C $BASE
    [[ $? -ne 0 ]] && echo "make failed!!" && exit
}

prn_time_info()
{
    echo "Estimated Time: Sec=${est_time_sec} HH:MM:SS=$est_time_hms curr_exec_time=$SECONDS"
}

get_estimated_time
mkdir -p $DIR/$LABEL 2>/dev/null
cp $CFG_FILE $DIR/$LABEL/
set_dco_conf 0
take_data "npdao_"

set_dco_conf 1
take_data "dco_"
