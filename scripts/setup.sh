#!/bin/bash

CFG_INC=config.inc

RIOT_EN=1
STACKLINE_RIOT=thirdparty/RIOT

CTK_EN=1
STACKLINE_CONTIKI=thirdparty/contiki

CTKNG_EN=0
STACKLINE_CONTIKING=thirdparty/contiki-ng

OT_EN=0
STACKLINE_OPENTHREAD=thirdparty/openthread

NS=1
AIRLINE_NS3=thirdparty/ns-3-dev-git

REL=debug

get_def_monitor_port()
{
    let MONITOR_PORT=65536-`id -u`-6
    [[ $MONITOR_PORT -lt 1024 ]] &&
        echo "MONITOR_PORT=$MONITOR_PORT is inappropriate!" &&
        exit 1
}

get_def_monitor_port

usage()
{
    echo "Usage: $0 <options>"
    echo \
"Options:
    --riot <0/1> ... enable RIOT OS <def: 0>
    --contiki <0/1> ... enable Contiki OS <def: 1>
    --contiki-ng <0/1> ... enable Contiki-ng OS <def: 0>
    --openthread <0/1> ... enable Openthread <def: 0>
    --ns3 <0/1> ... enable NS3 <def: 1>
    --monport <port-num> ... Monitor Port, (def: linux user-id)
    --mode <debug/release> ... build type <def: debug>
"
    exit 2
}

dump_config()
{
    echo "# Auto-generated file from setup.sh script. Do not modify manually." > $CFG_INC
    [[ $NS -eq 1 ]] && echo -en "AIRLINE=NS3\nAIRLINE_NS3=$AIRLINE_NS3\n" >> $CFG_INC
    [[ $CTK_EN -eq 1 ]] && echo "STACKLINE_CONTIKI=$STACKLINE_CONTIKI" >> $CFG_INC
    [[ $CTKNG_EN -eq 1 ]] && echo "STACKLINE_CONTIKING=$STACKLINE_CONTIKING" >> $CFG_INC
    [[ $RIOT_EN -eq 1 ]] && echo "STACKLINE_RIOT=$STACKLINE_RIOT" >> $CFG_INC
    [[ $OT_EN -eq 1 ]] && echo "STACKLINE_OPENTHREAD=$STACKLINE_OPENTHREAD" >> $CFG_INC
    echo "REL=$REL" >> $CFG_INC
    echo "BINDIR=bin" >> $CFG_INC
    echo "MONITOR_PORT=$MONITOR_PORT" >> $CFG_INC

    cat $CFG_INC
}

NC='\033[0m' # No Color
RED='\033[0;31m'
LRED='\033[0;33m'
GREEN='\033[0;32m'
echoc()
{
    clr="$1"; shift;
    printf "${clr}$*${NC}"
}

show_module()
{
    if [ $2 -eq 0 ]; then
        echoc $LRED "$1 Disabled"
        echo " ... use \"$3 1\" to enable"
    else
        echoc $GREEN "$1 Enabled"
        echo " ... use \"$3 0\" to disable"
    fi
}

show_config()
{
    echo -en "\n---=[Setup Configuration]=---\n"
    show_module "NS3" $NS "--ns3"
    show_module "Contiki" $CTK_EN "--contiki"
    show_module "Contiki-NG" $CTKNG_EN "--contiki-ng"
    show_module "RIOT" $RIOT_EN "--riot"
    show_module "OpenThread" $OT_EN "--openthread"
    echo "REL=$REL"
    echo -en "-----------------------------\n"
}

create_config()
{
    [[ "$1" == "" ]] && dump_config && return
    OPTS=`getopt -o h --long help,riot:,contiki:,contiki-ng:,ns3:,monport:,mode: \
        -n 'parse-options' -- "$@"`
    [[ $? -ne 0 ]] && usage
    eval set -- "$OPTS"
    while true; do
        case "$1" in
            --riot )       RIOT_EN="$2";       shift 2;;
            --contiki )    CTK_EN="$2";        shift 2;;
            --contiki-ng ) CTKNG_EN="$2";      shift 2;;
            --ns3 )        NS="$2";            shift 2;;
            --monport )    MONITOR_PORT="$2";  shift 2;;
            --mode )       REL="$2";           shift 2;;
            -h | --help )  usage;;
            -- ) shift; break ;;
            * ) break ;;
        esac
    done
    dump_config
}

chk_cmd_present()
{
	echo -en "checking [$1] ... "
	sudo apt install -f -y $1
}

chk_prerequisite()
{
	chk_cmd_present libc6-dev-i386
	chk_cmd_present libboost-all-dev
	chk_cmd_present unzip
	chk_cmd_present git
	chk_cmd_present make
	chk_cmd_present gcc
	chk_cmd_present gcc-9-multilib
	chk_cmd_present gcc-multilib
	chk_cmd_present python
	chk_cmd_present g++
	chk_cmd_present automake
	chk_cmd_present m4
	chk_cmd_present jq
	chk_cmd_present libtool
	chk_cmd_present graphviz
}

git_submodule_dload()
{
    [[ $1 -eq 1 ]] && echo "Submodule init [$2] ..." && \
        git submodule update --progress --init -- $2 && \
        cd $2 && git checkout master && cd -
}

git_download()
{
    git_submodule_dload $NS $AIRLINE_NS3
    git_submodule_dload $RIOT_EN $STACKLINE_RIOT
    git_submodule_dload $CTK_EN $STACKLINE_CONTIKI
    git_submodule_dload $CTKNG_EN $STACKLINE_CONTIKING
    git_submodule_dload $OT_EN $STACKLINE_OPENTHREAD

    if [ $NS -eq 1 ]; then
        cd thirdparty/ns-3-dev-git
        git submodule update --init --progress
        git submodule foreach git pull origin master
        cd -
    fi
}

create_config $*
. $CFG_INC

chk_prerequisite

git_download

if [ "$AIRLINE_NS3" != "" ]; then #Build NS3
	cd $AIRLINE_NS3
	./waf configure
	if [ $? -ne 0 ]; then
		echoc $RED "********* NS3 Compilation failed *********"
		exit 1
	fi
	cd -
fi

#Build Whitefield
make
if [ $? -ne 0 ]; then
	echoc $RED "********* Whitefield Compilation failed *********"
	exit 1
fi

show_config

echo "---=[Setup Success]=---"
