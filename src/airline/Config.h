#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <common.h>
#include <string>

namespace wf {
	class NodeConfig
	{
		private:
			string nodeExec;
			string capFile;
			int channel;
		public:
			void setNodeExecutable(const string path) {
				nodeExec = path;
			};
			void setNodeChannel(const int channel) {
				this->channel = channel;
			};
			void setNodeCaptureFile(const string path) {
				capFile = path;
			};
	};

	class Config
	{
		private:
			NodeConfig *nodeArray;
			int numOfNodes;
			map<string, string>keyval;

			int setNumberOfNodes(const int value)
			{
				clearNodeArray();
				nodeArray = new NodeConfig [value];
				numOfNodes = value;
				return SUCCESS;
			};

			int setNodeSetExec(const string exec, int beg, int end);
			int setNodeSetCapFile(const string path, int beg, int end);
			int setNodeSetChannel(const int chn, int beg, int end);

			void clearNodeArray(void)
			{
				if(nodeArray) {
					delete [] nodeArray;
					nodeArray = NULL;
					numOfNodes = 0;
				}
			};
			string getKeyRange(const string & keystr, int & beg_range, int & end_range);
			void set(string key, string val) { keyval[key]=val; };

		public:
			string get(string key) { return keyval[key]; };
			int setConfigurationFromFile(const char *filename);
			int getNumberOfNodes(void) { return numOfNodes; };
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
};

#endif //_CONFIG_H_

