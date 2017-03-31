#define _CONFIG_CC_

#include <common.h>
#include <Config.h>

int Config::setConfigurationFromFile(const char *fname)
{
  string line, key, value;
  ifstream infile(fname);

  try
  {
    while(getline(infile, line))
    {
      INFO << line << endl;
      istringstream is_line(line);
      if(!getline(is_line, key, '='))
      {
        INFO << line << endl;
        continue;
      }
      if(key[0] == '#') continue;
      if(!getline(is_line, value, '#')) continue;
      value = trim(value);
      key = trim(key);
      if(key == "numOfNodes") {
        setNumberOfNodes(stoi(value));
      } else if(key == "fieldX") {
        fieldX = stoi(value);
      } else if(key == "fieldY") {
        fieldY = stoi(value);
      } else if(key == "nodeExec") {
        nodeExec = value;
      } else if(key == "deploymentMode") {
        deploymentMode = value;
      } else {
        ERROR << "Not handled key " << key << endl;
        return FAILURE;
      }
    }
  } catch(exception & e) {
    ERROR << "Got exception " << e.what() << endl;
    return FAILURE;
  }
  return SUCCESS;
}
