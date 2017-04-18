#ifndef	_MSG_HANDLER_H_
#define	_MSG_HANDLER_H_

extern "C" {
#include "commline/commline.h"
}

typedef int msgrecv_cb_t(const msg_buf_t *mbuf);
void commline_thread(msgrecv_cb_t cb);

#endif	//	_MSG_HANDLER_H_
