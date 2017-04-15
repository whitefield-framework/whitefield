#define _MAIN_CC_

#include <signal.h>
#include <unistd.h>
#include <common.h>
#include <Manager.h>
extern "C" {
#include "commline/commline.h"
}

void sig_handler(int signum)
{
	if(signum > 1) {
		ERROR << "Caught signal " << signum << endl;
	}
	cl_cleanup();
	exit(signum);
}

void exec_forker(void)
{
	char *cmdname=getenv("FORKER");
	if(!cmdname) {
		ERROR << "Could not find forker env var\n";
		sig_handler(1);
	}
	if(0 == fork()) {
		char *argv[10] = {
			cmdname,
			NULL,
		};
		execv(cmdname, argv);
		ERROR << "Could not execv " << cmdname << ". Check if the forker cmdname/path is correct.Aborting..." << endl;
		sig_handler(1);
	}
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
		sig_handler(1);
	}
	if(SUCCESS != WF_config.setConfigurationFromFile(argv[1])) {
		ERROR << "Terminating...\n"; 
		sig_handler(1);
	}
	exec_forker();
	Manager WF_mgr(WF_config);
	sig_handler(0);
	return 0;
}

