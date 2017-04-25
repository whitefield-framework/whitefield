#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "common.h"
extern "C" {
#include "commline/commline.h"
}

void al_handle_cmd(msg_buf_t *mbuf);

#endif	// _COMMAND_H_
