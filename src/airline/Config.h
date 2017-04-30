#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <common.h>

namespace wf {
	class Config
	{
		private:
			Nodeinfo *nodeArray;
			int numOfNodes;
			map<string, string>keyval;

			int setNumberOfNodes(const int value);
			int setNodeSetExec(const string exec, int beg, int end);
			int setNodeSetCapFile(const string path, int beg, int end);
			int setNodePosition(const string position, int beg, int end);

			void clearNodeArray(void);
			string getKeyRange(const string & keystr, int & beg_range, int & end_range);
			void set(string key, string val) { keyval[key]=val; };

		public:
			Nodeinfo *get_node_info(uint16_t id);
			string get(string key) { return keyval[key]; };
			string get(string key, string def) { 
				string out = keyval[key]; 
				return out.empty()?def:out;
			};
			int setConfigurationFromFile(const char *filename);
			int getNumberOfNodes(void) { return numOfNodes; };
			void spawnStackline(const uint16_t nodeID);
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

