#!/bin/bash

DIR=`dirname $0`
. $DIR/helpers.sh

function init_node()
{
    echo "---[initing openthread node $1]---"
    echo -en "version\npanid 0x1234\nifconfig up\nthread start\n" | $DIR/wfshell native_shell $1
    [[ $? -ne 0 ]] && echo "problem initing ..." && exit

}

get_node_list
for((i=0;i<$nodecnt;i++)); do
    init_node $i
done
