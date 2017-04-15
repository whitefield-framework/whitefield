#ifndef	_CL_MSGQ_H_
#define	_CL_MSGQ_H_

int msgq_init(const uint8_t flags);
void msgq_cleanup(void);
int msgq_recvfrom(const long mtype, msg_buf_t *mbuf, uint16_t len);
int msgq_sendto(const long mtype, msg_buf_t *mbuf, uint16_t len);

#endif //_CL_MSGQ_H_
