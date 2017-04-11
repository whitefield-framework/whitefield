#define	_CL_MSGQ_C_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

/* sys v msgq related headers */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <commline.h>
#include <cl_msgq.h>

#define	ERROR printf
#define	INFO printf
#define	WARN printf

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
	return CL_SUCCESS;
}

