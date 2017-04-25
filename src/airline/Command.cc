#define	_COMMAND_CC_

#include "Command.h"
#include "mac_stats.h"

int cmd_mac_stats(uint16_t nodeid, char *buf, int buflen)
{
	return wf::Macstats::get_summary(nodeid, buf, buflen);
}

void al_handle_cmd(msg_buf_t *mbuf)
{
	if(0) { } 
	HANDLE_CMD(mbuf, cmd_mac_stats)
	else {
		mbuf->len = sprintf((char*)mbuf->buf, "AL_INVALID_CMD(%s)", mbuf->buf);
	}
}

