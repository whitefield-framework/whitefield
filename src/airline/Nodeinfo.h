#ifndef _NODEINFO_H_
#define _NODEINFO_H_

#include <common.h>
#include <mac_stats.h>

namespace wf {
	class Nodeinfo : public Macstats
	{
		private:
			string nodeExec;
			string capFile;
		public:
			string getNodeExecutable(void) { return nodeExec; };
			void setNodeExecutable(const string path) {
				nodeExec = path;
			};
			void setNodeCaptureFile(const string path) {
				capFile = path;
			};
	};
}	//namespace wf

#endif// _NODEINFO_H_
