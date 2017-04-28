#define	_SL_COMMAND_C_

// This file is included from thirdparty stackline OS such as Contiki (see contiki/platform/whitefiled/command.c)
// It sort of provides virtual abstract interfaces for different commands i.e. it mandates the stackline OS platform 
// to mandatorily implement these commands. This is essential to have a common OAM interfaces across different 
// stackline implementations.

#include "commline/commline.h"

void sl_handle_cmd(msg_buf_t *mbuf)
{
	INFO("Handle monitor command <TODO>\n");
	if(0) { } 
	HANDLE_CMD(mbuf, cmd_rpl_stats)
	HANDLE_CMD(mbuf, cmd_def_route)
	HANDLE_CMD(mbuf, cmd_route_table)
	HANDLE_CMD(mbuf, cmd_rtsize)
	HANDLE_CMD(mbuf, cmd_node_osname)
	HANDLE_CMD(mbuf, cmd_ipv6_stats)
	HANDLE_CMD(mbuf, cmd_nd6_stats)
	HANDLE_CMD(mbuf, cmd_icmp_stats)
	HANDLE_CMD(mbuf, cmd_udp_stats)
	HANDLE_CMD(mbuf, cmd_tcp_stats)
	else {
		mbuf->len = sprintf((char*)mbuf->buf, "SL_INVALID_CMD(%s)", mbuf->buf);
	}   
}

