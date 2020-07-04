/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     airline
 * @{
 *
 * @file
 * @brief       Startup functions for whitefield
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _MAIN_CC_

#include <signal.h>
#include <unistd.h>
#include <common.h>
#include <sys/prctl.h>
#include <Manager.h>
extern "C" {
#include "commline/commline.h"
}

void sig_handler(int signum)
{
	if(signum > 1) {
		CINFO << "Airline Caught signal " << signum << endl;
	}
	cl_cleanup();
	CINFO << "Sayonara " << signum << "...\n";
	exit(signum);
}

void exec_forker(void)
{
	char *cmdname=getenv("FORKER");
	if(!cmdname) {
		CERROR << "Could not find forker env var\n";
		sig_handler(1);
	}
	if(0 == fork()) {
		char *argv[10] = {
			cmdname,
			NULL,
		};

		/* When whitefield exits, then send inform forker */ 
		prctl(PR_SET_PDEATHSIG, SIGINT);

		execv(cmdname, argv);
		CERROR << "Could not execv " << cmdname 
              << ". Check if the forker cmdname/path is correct.Aborting..."
              << endl;
		sig_handler(1);
	}
}

#if 0
ofstream g_errout;
void redirect_log(void)
{
	char outfile[512];
	if(!getenv("LOGPATH")) return;
	snprintf(outfile, sizeof(outfile), "%s/airline_error.log", getenv("LOGPATH"));

	g_errout.open(outfile);
//	streambuf *cerrbuf=cerr.rdbuf();
	cerr.rdbuf(g_errout.rdbuf());
}
#endif

wf::Config WF_config;
int main(const int argc, const char *argv[])
{
	if(argc<2)
	{
		CINFO << "Usage: " << argv[0] << " <config_file>\n";
		return 0;
	}
	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGTERM, sig_handler);
	//signal(SIGSEGV, sig_handler);
	signal(SIGCHLD, sig_handler);

	if(SUCCESS != cl_init(MTYPE(AIRLINE, CL_MGR_ID), CL_CREATEQ)) {
		CERROR << "Whitefield is already running\n";
		sig_handler(1);
	}
	if(SUCCESS != WF_config.setConfigurationFromFile(argv[1])) {
		CERROR << "Terminating...\n"; 
		sig_handler(1);
	}
	//redirect_log();
	exec_forker();
	Manager WF_mgr(WF_config);
	sig_handler(0);
	return 0;
}

