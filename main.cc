#define _MAIN_CC_
#include <common.h>
#include <Config.h>
#include <Manager.h>

Config wf_config;
int main(const int argc, const char *argv[])
{
  if(argc<2)
  {
    INFO << "Usage: " << argv[0] << " <config_file>\n";
    return 0;
  }
  wf_config.setConfigurationFromFile(argv[1]);
  INFO << wf_config.getNumberOfNodes() << endl;
  Manager mgr(wf_config);
  return 0;
}
