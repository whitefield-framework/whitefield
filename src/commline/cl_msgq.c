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

#define	MAKE_MTYPE(MTYPE, LINE, ID)	\
	(MTYPE) = LINE << 16;\
	(MTYPE) |= ID;

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
		ERROR("could not get msgq key:0x%x\n", key);
		return CL_FAILURE;
	}
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

int msgq_recvfrom(const long mtype, msg_buf_t *mbuf, uint16_t len)
{
	int ret;

	mbuf->len=0;
	ret = msgrcv(gMsgQ_id, (void*)mbuf, len, mtype, IPC_NOWAIT);
	if(ret<0) {
		if(ENOMSG == errno) return CL_SUCCESS;
		return CL_FAILURE;
	}
	if(ret < sizeof(msg_buf_t)) 
	{
		ERROR("some problem ... msgrcv length(%d) not enough\n", ret);
		return CL_FAILURE;
	}
	return CL_SUCCESS;
}

int msgq_sendto(const long mtype, msg_buf_t *mbuf, uint16_t len)
{
	mbuf->mtype=mtype;
	if(msgsnd(gMsgQ_id, (void*)mbuf, len, 0)<0) {
		ERROR("msgsend failed!\n");
		return CL_FAILURE;
	}
	return CL_SUCCESS;
}

