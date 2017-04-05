#define _CONFIG_CC_

#include <common.h>
#include <Config.h>
#include <string.h>

string wf::Config::getKeyRange(const string & keystr, int & beg_range, int & end_range)
{
	istringstream keyline(keystr);
	string key=keystr;
	const char *ptr;

	beg_range = 0;
	end_range = numOfNodes-1;
	if(!(ptr=strchr(keystr.c_str(), '['))) return key; //No range specified in the key
	getline(keyline, key, '[');
	ptr++;
	beg_range = atoi(ptr);
	if(!(ptr = strchr(ptr, '-'))) {
		end_range = beg_range;
		return key;
	}
	ptr++;
	end_range = atoi(ptr);
	if(end_range <= beg_range) {
		ERROR << "invalid range:" << keystr << endl;
		return "";
	}
	return key;
}

int wf::Config::setNodeExec(const string exec, int beg, int end)
{
	int i;
	for(i=beg;i<=end;i++) {
		nodeArray[i].setNodeExecutable(exec);
	}
	return SUCCESS;
}

int wf::Config::setNodeCapFile(const string path, int beg, int end)
{
	int i;
	for(i=beg;i<=end;i++) {
		nodeArray[i].setNodeCaptureFile(path);
	}
	return SUCCESS;
}

int wf::Config::setConfigurationFromFile(const char *fname)
{
	int beg_range, end_range;
	string line, key, value;

	try
	{
		ifstream infile(fname);
		if(!infile) {
			ERROR << "Could not open file " << fname << endl;
			return FAILURE;
		}
		while(getline(infile, line))
		{
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
			key = getKeyRange(key, beg_range, end_range);
			//INFO << "--- key=" << key << " beg=" << beg_range << " end=" << end_range << " val=" << value << endl;
			if(key == "numOfNodes") {
				setNumberOfNodes(stoi(value));
			} else if(key == "fieldX") {
				fieldX = stod(value);
			} else if(key == "fieldY") {
				fieldY = stod(value);
			} else if(key == "nodeExec") {
				setNodeExec(value, beg_range, end_range);
			} else if(key == "captureFile") {
				setNodeCapFile(value, beg_range, end_range);
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

