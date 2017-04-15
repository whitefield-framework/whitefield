#define _MAIN_CC_

#include <signal.h>
#include <common.h>
#include <Manager.h>
extern "C" {
#include "commline/commline.h"
}

void sig_handler(int signum)
{
	cl_cleanup();
	exit(0);
}

wf::Config WF_config;
int main(const int argc, const char *argv[])
{
	if(argc<2)
	{
		INFO << "Usage: " << argv[0] << " <config_file>\n";
		return 0;
	}
	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGSEGV, sig_handler);

	if(CL_SUCCESS != cl_init(CL_CREATEQ)) {
		ERROR << "Failure creating commline\n";
		return FAILURE;
	}
	char * ptr = 0x0;
	strcpy(ptr, "rahul");
	if(SUCCESS != WF_config.setConfigurationFromFile(argv[1])) {
		ERROR << "Terminating...\n"; 
		exit(1);
	}
	Manager WF_mgr(WF_config);
	sig_handler(0);
	return 0;
}

