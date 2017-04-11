#define _MAIN_CC_
#include <common.h>
#include <Manager.h>

wf::Config WF_config;
int main(const int argc, const char *argv[])
{
  if(argc<2)
  {
    INFO << "Usage: " << argv[0] << " <config_file>\n";
    return 0;
  }
  if(SUCCESS != WF_config.setConfigurationFromFile(argv[1])) {
	  ERROR << "Terminating...\n"; 
	  exit(1);
  }
  Manager WF_mgr(WF_config);
  return 0;
}
