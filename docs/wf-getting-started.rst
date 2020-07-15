###############
Getting Started
###############

Updating Configuration
----------------------
The first step to do post setup is to update the configuration.
The `config` folder contains all the configuration files. Check sample configuration `wf.cfg`_.
Check detailed `configuration`_ options.

.. _configuration: https://github.com/whitefield-framework/whitefield/blob/master/docs/wf-config-help.rst
.. _wf.cfg: https://github.com/whitefield-framework/whitefield/blob/master/config/wf.cfg

Starting Whitefield
-------------------
::

    cd whitefield
    ./invoke_whitefield config/wf.cfg

Stopping Whitefield
-------------------
::

    ./scripts/wfshell stop_whitefield

wfshell
-------
::

    ❯ ./scripts/wfshell
    wfsh# <tab><tab>
    cmd_config_info        cmd_ipv6_stats         cmd_node_osname        cmd_rtsize             cmd_udp_stats          path_downstream        
    cmd_def_route          cmd_mac_stats          cmd_node_position      cmd_set_node_position  exit                   path_upstream          
    cmd_get_udpapp_stat    cmd_nd6_stats          cmd_route_table        cmd_start_udp          help                   plot_network_graph     
    cmd_icmp_stats         cmd_node_exec          cmd_rpl_stats          cmd_tcp_stats          native_shell           stop_whitefield        

wfshell sends the command to Airline or Stackline and prints the response. The command can be sent to all nodes or to specific nodes.
::

    ./scripts/wfshell <cmd_foobar> ... executes cmd_foobar on all the nodes
    ./scripts/wfshell <cmd_foobar> <node_id> ... executes cmd_foobar on node_id only
