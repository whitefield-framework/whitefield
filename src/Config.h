#ifndef _CONFIG_H_
#define _CONFIG_H_

class NodeConfig
{
	private:
		string nodeExec;
		string capFile;
	public:
		int setNodeExecutable(const string path)
		{
			nodeExec = path;
			return SUCCESS;
		};
		int setNodeCaptureFile(const string path)
		{
			capFile = path;
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

		int setNodeExec(const string exec, int beg, int end);
		int setNodeCapFile(const string path, int beg, int end);

		void clearNodeArray(void)
		{
			if(nodeArray) {
				delete [] nodeArray;
				nodeArray = NULL;
			}
		};
		string getKeyRange(const string & keystr, int & beg_range, int & end_range);

	public:
		int setConfigurationFromFile(const char *filename);
		int getNumberOfNodes(void) {
			return numOfNodes;
		};
		Config() 
		{
			nodeArray=NULL;
			numOfNodes=0;
		};
		~Config()
		{
			clearNodeArray();
		}
};

#endif //_CONFIG_H_

