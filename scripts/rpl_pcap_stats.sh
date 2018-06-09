#!/bin/bash

ICMP_RPL_TYPE=155
DIS_CODE=0x00
DIO_CODE=0x01
DAO_CODE=0x02
DAOACK_CODE=0x03
MCAST16_ADDR=0xffff
JSON_OUT="stats.json"

chk_prerequisites()
{
    while [ "$1" != "" ]; do
        which "$1" >/dev/null
        [[ $? -ne 0 ]] && echo "$1 command not found" && exit 1
        shift
    done
}

usage()
{
    echo "Usage: $0"
    echo "* -r|--read <pcap_fname>"
    echo "  -w|--write <json_fname>"
#    echo "  -l|--latency <num> .. in millisec"
    echo "* denotes mandatory args"
    exit
}

parse_cmdargs()
{
    chk_prerequisites tshark capinfos
    OPTS=`getopt -n 'parse-options' -o r:w: --long read:,write: -- "$@"`
    #echo "$OPTS"
    if [ $? != 0 ] ; then echo "Failed parsing options." >&2 ; usage ; fi
    eval set -- "$OPTS"
    while true; do
        case "$1" in
            -r | --read ) PCAP_FILE="$2";  shift 2;;
            -w | --write ) JSON_OUT="$2";  shift 2;;
            -- ) shift; break ;;
            * ) break ;;
        esac
    done
    [[ "$PCAP_FILE" == "" ]] && usage
    [[ -f "$JSON_OUT" ]] && rm "$JSON_OUT"
}

get_pkt_details()
{
    echo "filtering [$1] ..."
    tshark -nr $PCAP_FILE -Y "$1" -w $TMP_CAP 2>/dev/null
    capinfos $TMP_CAP -icd -Tm | tail -1 > $TMP_TXT
    num_of_pkts=`cut -f2 -d, $TMP_TXT`
    pkts_sz=`cut -f3 -d, $TMP_TXT`

    cat <<EOF >> $JSON_OUT
    "$2": {
        "num_of_pkts": "$num_of_pkts",
        "pkts_sz": "$pkts_sz"
    },
EOF
}

get_pcap_summary()
{
    echo "summarizing..."
    capinfos $PCAP_FILE -u -Tm | tail -1 > $TMP_TXT
    duration=`cut -f2 -d, $TMP_TXT`

    cat <<EOF >> $JSON_OUT
    "$1": {
        "duration": "$duration"
    }
EOF
}

main()
{
    TMP_DIR=out
    TMP_CAP=$TMP_DIR/tmp.cap
    TMP_TXT=$TMP_DIR/tmp.txt
    [[ ! -d "$TMP_DIR" ]] && mkdir -p "$TMP_DIR"
    echo "{" > $JSON_OUT
    get_pkt_details "icmpv6.type==$ICMP_RPL_TYPE and icmpv6.code==$DIS_CODE && wpan.dst16==$MCAST16_ADDR" "dis_mcast"
    get_pkt_details "icmpv6.type==$ICMP_RPL_TYPE and icmpv6.code==$DIS_CODE && wpan.dst16!=$MCAST16_ADDR" "dis_ucast"
    get_pkt_details "icmpv6.type==$ICMP_RPL_TYPE and icmpv6.code==$DIO_CODE && wpan.dst16==$MCAST16_ADDR" "dio_mcast"
    get_pkt_details "icmpv6.type==$ICMP_RPL_TYPE and icmpv6.code==$DIO_CODE && wpan.dst16!=$MCAST16_ADDR" "dio_ucast"
    get_pkt_details "icmpv6.type==$ICMP_RPL_TYPE and icmpv6.code==$DAO_CODE && wpan.dst16!=$MCAST16_ADDR" "dao_ucast"
    get_pcap_summary "auxinfo"
    echo "}" >> $JSON_OUT
    echo "done. check $JSON_OUT"
}

parse_cmdargs $*
main
