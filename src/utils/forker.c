#define	_FORKER_C_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "commline/commline.h"

void redirect_stdout_to_log(int nodeid)
{
	int fd;
	char logfile[512];

	snprintf(logfile, sizeof(logfile), "%s/node_%04d.log", getenv("LOGPATH")?getenv("LOGPATH"):"log", nodeid);
	fd = open(logfile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd > 0) {
		dup2(fd, 1);
		dup2(fd, 2);
		close(fd);
	}
}

#define	MAX_CHILD_PROCESS	32000
pid_t gChildProcess[MAX_CHILD_PROCESS];
void fork_n_exec(char *buf)
{
	char *argv[20] = {NULL};
	int i=0, nodeid;
	char *ptr=NULL;

	do {
		if(ptr) {
			*ptr = '\0';
			buf = ++ptr;
		}
		argv[i++] = buf;
	} while((ptr=strchr(buf, '|')));
	argv[i] = NULL;

	if(i<1) {
		ERROR("Insufficient command exec info\n");
		return;
	}
	nodeid = atoi(argv[1]);

	gChildProcess[nodeid] = fork();
	if(0 == gChildProcess[nodeid]) {
		redirect_stdout_to_log(nodeid);
		execv(argv[0], argv);
		ERROR("Could not execv [%s]. Check if the cmdname/path is correct.Aborting...\n", argv[0]);
	} else if(gChildProcess[nodeid] < 0) {
		ERROR("fork failed!!!\n");
	}
}

void killall_childprocess(void)
{
	int i;
	for(i=0;i<MAX_CHILD_PROCESS;i++) {
		if(gChildProcess[i] <= 0) continue;
		kill(gChildProcess[i], SIGKILL);
	}
}

void wait_on_q(void)
{
	uint8_t buf[sizeof(msg_buf_t)+COMMLINE_MAX_BUF];
	msg_buf_t *mbuf=(msg_buf_t*)buf;

	while(1)
	{
		if(CL_SUCCESS != cl_recvfrom_q(MTYPE(FORKER,CL_MGR_ID), mbuf, sizeof(buf))) {
			break;
		}
		if(mbuf->len) {
			fork_n_exec((char *)mbuf->buf);
		}
		usleep(1000);
	}
	killall_childprocess();
	INFO("Quitting forker process\n");
}

extern int start_monitor_thread(void);

int main(void)
{
	if(CL_SUCCESS != cl_init(CL_ATTACHQ)) {
		ERROR("forker: failure to cl_init()\n");
		return 1;
	}
	if(CL_SUCCESS != start_monitor_thread()) {
		ERROR("start_monitor_thread failed... exiting process!!\n");
		return 1;
	}
	wait_on_q();
	return 0;
}

