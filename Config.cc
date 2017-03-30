#define _CONFIG_CC_

#include <common.h>
#include <Config.h>

int Config::setNumberOfNodes(const int value)
{
  numOfNodes=value;
  return SUCCESS;
}

int Config::setConfigurationFromFile(const char *fname)
{
  int ret;
  string line, key, value;
  ifstream infile(fname);

  while(getline(infile, line))
  {
    istringstream is_line(line);
    if(!getline(is_line, key, '='))
    {
      std::cout << line << std::endl;
      continue;
    }
    if(key[0] == '#') continue;
    if(!getline(is_line, value)) continue;
    INFO << "key=" << key << " value=" << value << endl;
    ret=FAILURE;
    if(key=="nodes")
    {
        ret=setNumberOfNodes(stoi(value));
    }
    else
    {
      ERROR << "Not handled key " << key << endl;
    }
    if(FAILURE == ret) break;
  }
  return ret;
}
