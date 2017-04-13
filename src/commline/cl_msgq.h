#ifndef	_CL_MSGQ_H_
#define	_CL_MSGQ_H_

int msgq_init(const uint8_t flags);
void msgq_cleanup(void);
int msgq_recvfrom(const uint16_t srcid, uint8_t *buf, uint16_t *buflen);

#endif //_CL_MSGQ_H_
