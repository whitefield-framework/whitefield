#!/bin/bash

DIR=`dirname $0`
. "$DIR/helpers.sh"
SNAPSH="$DIR/aggregate_stats.sh"

function usage()
{
    echo "$0 <options>"
    cat <<EOH
    --interval <time_in_sec> ... Time series interval (def=5sec)
    --cumu <0|1> ... use cumulative statistics (def=1)
    --cmd <cmd-string> ... for e.g., cmd_rpl_stats ... check wfshell for other commands
    --cnt <cnt-of-readings> ... max count of readings to take
    --field <field-string> ... for e.g., .rpl_stats.parent_switch
    --nodes <nodeset> ... set of nodes for e.g., 0-10 or 1

    * - indicates mandatory parameters
EOH
    exit 2
}

function err()
{
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    printf "${RED}ERROR: $*${NC}\n"
}

function set_interval()
{
    P_INTERVAL="$1"
    [[ $P_INTERVAL -lt 2 ]] && echo "Too low interval $P_INTERVAL" && exit 1
}

function set_cumu()
{
    P_CUMU=$1
    [[ ! $P_CUMU =~ 0|1 ]] && echo "Incorrect cumu arg($1) provided" && exit 1
}

parse_args()
{
    opts=$(getopt \
        -o c \
        --long cumu: \
        --long interval: \
        --long cmd: \
        --long cnt: \
        --long field: \
        --long nodes: \
        -- "$@")

    [ $? -eq 0 ] || { 
        echo "Incorrect options provided"
        exit 1
    }
    eval set -- "$opts"
    P_INTERVAL=5
    P_CUMU=1
    P_CNT=1

    while true; do
        opt="$1"
        shift;
        case "$opt" in
        --interval)
            set_interval "$1"
            ;;
        --cumu)
            set_cumu "$1"
            ;;
        --field)
            P_FIELD="$1"
            ;;
        --cmd)
            P_CMD="$1"
            ;;
        --nodes)
            P_NODES="$1"
            ;;
        --cnt)
            P_CNT="$1"
            ;;
        --)
            break
            ;;
        esac
        shift
    done
    [[ "$P_CMD" == "" ]] && err "Stat CMD not provided" && usage
    [[ "$P_FIELD" == "" ]] && err "Stat FIELD not provided" && usage
}

function get_data()
{
    outjson=`$SNAPSH $P_CMD $P_FIELD $P_NODES`
    [[ $? -ne 0 ]] && err "$P_CMD $P_FIELD failed" && exit 1
    row_tot=`echo $outjson | jq -r '.total'`
    row_avg=`echo $outjson | jq -r '.average'`
    elap_time
    IFS=: read h m s <<<"${wf_elap_time%.*}"
    row_sec=$((10#$s+10#$m*60+10#$h*3600))
}

parse_args $@

for ((i = 0; i < $P_CNT; i++)); do
    get_data
    echo "$row_sec,$row_tot,$row_avg"
    sleep $P_INTERVAL
done


