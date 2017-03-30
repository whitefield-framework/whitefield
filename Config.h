#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config
{
private:
  int numOfNodes;
  int setNumberOfNodes(const int value);
public:
  int setConfigurationFromFile(const char *filename);
  int getNumberOfNodes(void) { 
    return numOfNodes;
  };
};

#endif //_CONFIG_H_
