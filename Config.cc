#define _CONFIG_CC_

#include <common.h>
#include <Config.h>
#include <string.h>

string Config::getKeyRange(const string & keystr, int & beg_range, int & end_range)
{
	istringstream keyline(keystr);
	string key=keystr;
	const char *ptr;

	beg_range = end_range = 0;
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

int Config::setConfigurationFromFile(const char *fname)
{
	int beg_range, end_range;
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
			getKeyRange(key, beg_range, end_range);
			INFO << "--- key=" << key << " beg=" << beg_range << " end=" << end_range << " val=" << value << endl;
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

