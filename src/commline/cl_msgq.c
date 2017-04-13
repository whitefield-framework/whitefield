#define	_CL_MSGQ_C_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

/* sys v msgq related headers */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <commline.h>
#include <cl_msgq.h>

int gMsgQ_id=-1;
key_t get_msgq_key(void)
{
	char buf[512];
	return ftok(getcwd(buf, sizeof(buf)), 0xab);
}

int msgq_init(const uint8_t flags)
{
	key_t key;
	int msgflag=0666 | IPC_CREAT;

	key=get_msgq_key();
	if(-1==key) {
		ERROR("could not get a valid msgq key\n");
		return CL_FAILURE;
	}
	if(flags & CL_CREATEQ) {
		msgflag |= IPC_EXCL;
	}
	gMsgQ_id = msgget(key, msgflag);
	if(-1 == gMsgQ_id) {
		ERROR("could not get msgq\n");
		return CL_FAILURE;
	}
	INFO("Msgq creation success\n");
	return CL_SUCCESS;
}

void msgq_cleanup(void)
{
	if(msgctl(gMsgQ_id, IPC_RMID, NULL)<0) {
		ERROR("Failure deleting the msgq!\n");
		return;
	}
	INFO("msgq deleted\n");
}

typedef struct _cl_msg_ {
	long mtype;
	uint8_t buf[COMMLINE_MAX_BUF];
} cl_msg_t;

int msgq_recvfrom(const uint16_t srcid, uint8_t *buf, uint16_t *buflen)
{
	cl_msg_t msg;
	msg.mtype = srcid;
	int ret;
	*buflen=0;
	ret = msgrcv(gMsgQ_id, (void*)&msg, sizeof(msg), srcid, IPC_NOWAIT);
	if(ret<0) {
		if(ENOMSG == errno) return CL_SUCCESS;
		return CL_FAILURE;
	}
	if(ret <= sizeof(long)) 
	{
		ERROR("some problem ... msgrcv length(%d) not enough\n", ret);
		return CL_FAILURE;
	}
	memcpy(buf, msg.buf+sizeof(long), ret - sizeof(long));
	*buflen = ret - sizeof(long);
	return CL_SUCCESS;
}

