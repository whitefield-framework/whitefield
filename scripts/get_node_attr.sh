#!/bin/bash

# Not to be called directly. Called through network_graph.sh

# This scripts returns the node attributes to be used by plot_network_graph.
# This may be used for other purpose in the future, hence the output in json
# format.

# Currently this scripts checks the node executable and returns the node-color.

DIR=`dirname $0`
[[ "$1" == "" ]] && echo "Usage: $0 <nodeid>" && exit 1

# https://graphviz.org/doc/info/colors.html
get_node_color()
{
    nexec=`$DIR/wfshell cmd_node_exec $1`
    bin=`basename "${nexec/ */}"`
    color="${DEFCOLOR-greenyellow}"
    if [[ $bin =~ .*udp-server.* ]]; then
        color="${BRCOLOR-darkgreen}"
    fi

    echo "{ \"color\": \"$color\" }"
}
