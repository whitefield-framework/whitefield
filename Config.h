#ifndef _CONFIG_H_
#define _CONFIG_H_

class NodeConfig
{
private:
  string nodeExec;
public:
  int setNodeExecutable(const string path)
  {
    nodeExec = path;
    return SUCCESS;
  };
};

class Config
{
private:
  NodeConfig *nodeArray;
  int fieldX, fieldY;
  int numOfNodes;
  string deploymentMode;
  string nodeExec;

  int setNumberOfNodes(const int value)
  {
    clearNodeArray();
    nodeArray = new NodeConfig [value];
    numOfNodes = value;
    return SUCCESS;
  };

  void clearNodeArray(void)
  {
    if(nodeArray) {
      delete [] nodeArray;
      nodeArray = NULL;
    }
  };

public:
  int setConfigurationFromFile(const char *filename);
  int getNumberOfNodes(void) {
    return numOfNodes;
  };
  Config() 
  {
	  nodeArray=NULL;
  };
  ~Config()
  {
    clearNodeArray();
  }
};

#endif //_CONFIG_H_

