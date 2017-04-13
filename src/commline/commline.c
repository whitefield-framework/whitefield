#define	_COMMLINE_C_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <commline.h>
#include <errno.h>
#include <cl_msgq.h>

int cl_init(const uint8_t flags)
{
	return msgq_init(flags);
}

void cl_cleanup(void)
{
	msgq_cleanup();
}

int cl_sendto_q(const uint8_t *buf, const uint16_t buflen, const uint16_t srcid, const uint16_t dstid)
{
	return CL_SUCCESS;
}

int cl_recvfrom_q(const uint16_t srcid, uint8_t *buf, uint16_t *buflen)
{
	if(!buf || !buflen) { 
		ERROR("invalid parameters passed buf:%p, buflen:%p\n", buf, buflen);
		return CL_FAILURE;
	}
	return msgq_recvfrom(srcid, buf, buflen);
}

