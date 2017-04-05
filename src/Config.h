#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <map>

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

#if 0
	class NodeSet
	{
	private:
		NodeConfig *nodeArray;
		int nodeCnt;
		void clearNodeArray(void)
		{
			if(nodeArray) {
				delete [] nodeArray;
			}
			nodeArray = NULL;
			nodeCnt = 0;
		};
	public:
		int setCount(const int cnt) {
			nodeArray = new NodeConfig [cnt];
			nodeCnt = cnt;
		};
		NodeSet() {
			nodeCnt=0;
		};
		~NodeSet() {
		};
	};
#endif

	class Config
	{
		private:
			NodeConfig *nodeArray;
			int numOfNodes;
			std::map<string, string>keyval;

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

		public:
			void set(string key, string val) { keyval[key]=val; };
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

