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
		INFO << "Caught signal " << signum << endl;
	}
	cl_cleanup();
	INFO << "Sayonara...\n";
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

ofstream g_errout;
void redirect_log(void)
{
	char outfile[256];
	if(!getenv("LOGPATH")) return;
	snprintf(outfile, sizeof(outfile), "%s/airline_error.log", getenv("LOGPATH"));

	g_errout.open(outfile);
//	streambuf *cerrbuf=cerr.rdbuf();
	cerr.rdbuf(g_errout.rdbuf());
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
		ERROR << "Possibly because whitefield is already running or there was problem clearing msgq during last exit\n";
		ERROR << "To clear msgq use cmd 'ipcrm -Q <key>', you ll see key 0x.. in err msg above\n";
		sig_handler(1);
	}
	if(SUCCESS != WF_config.setConfigurationFromFile(argv[1])) {
		ERROR << "Terminating...\n"; 
		sig_handler(1);
	}
	redirect_log();
	exec_forker();
	Manager WF_mgr(WF_config);
	sig_handler(0);
	return 0;
}

