#define _MAIN_CC_
#include <common.h>
#include <Config.h>
#include <Manager.h>

Config WF_config;
int main(const int argc, const char *argv[])
{
  if(argc<2)
  {
    INFO << "Usage: " << argv[0] << " <config_file>\n";
    return 0;
  }
  WF_config.setConfigurationFromFile(argv[1]);
  INFO << WF_config.getNumberOfNodes() << endl;
  Manager WF_mgr(WF_config);
  return 0;
}
