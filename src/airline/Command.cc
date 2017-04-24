#define	_COMMAND_CC_

#include "Command.h"
#include "mac_stats.h"

#define	HANDLE_CMD(MBUF, CMD)	\
	else if(!strcasecmp((char*)(MBUF)->buf, #CMD))	\
	{\
		(MBUF)->len = CMD(mbuf->src_id, (char*)(MBUF)->buf, COMMLINE_MAX_BUF);\
	}

int cmd_mac_stats(uint16_t nodeid, char *buf, int buflen)
{
	return wf::Macstats::get_summary(nodeid, buf, buflen);
}

void handle_cmd(msg_buf_t *mbuf)
{
	if(0) { } 
	HANDLE_CMD(mbuf, cmd_mac_stats)
	else {
		mbuf->len = sprintf((char*)mbuf->buf, "INVALID_CMD(%s)", mbuf->buf);
	}
}

