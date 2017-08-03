/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     airline
 * @{
 *
 * @file
 * @brief       Configuration handler
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _CONFIG_CC_

#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>
extern "C" {
#include "commline/commline.h"
}

using namespace wf;
extern void sig_handler(int);
//This interface is called from AirlineManager...
void Config::spawnStackline(const uint16_t nodeID)
{
	uint8_t buf[sizeof(msg_buf_t) + COMMLINE_MAX_BUF];
	msg_buf_t *mbuf = (msg_buf_t*)buf;
	int len=0;
	string cmd = nodeArray[nodeID].getNodeExecutable();

	if(cmd.empty()) {
		ERROR << "No Stackline executable configured for nodeID:" << nodeID << endl;
		sig_handler(1);
	}

	INFO << "spawning node:" << nodeID 
		 << " Exec: " << cmd
		 << "\r";
	len = snprintf((char *)mbuf->buf, COMMLINE_MAX_BUF, "%s|%d", cmd.c_str(), nodeID);
	mbuf->len = len;
	if(CL_SUCCESS != cl_sendto_q(MTYPE(FORKER, CL_MGR_ID), mbuf, len + sizeof(msg_buf_t))) {
		ERROR << "Failure sending command to forker\n";
	}
	if(nodeID == getNumberOfNodes()-1) {
		INFO << "\nAll nodes started.\n";
	}
}

string Config::getKeyRange(const string & keystr, int & beg_range, int & end_range)
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
	if(end_range >= numOfNodes) {
		end_range = numOfNodes-1;
	}
	return key;
}

int Config::setNodeSetExec(const string exec, int beg, int end)
{
	int i;
	for(i=beg;i<=end;i++) {
		nodeArray[i].setNodeExecutable(exec);
	}
	return SUCCESS;
}

int Config::setNodeSetCapFile(const string path, int beg, int end)
{
	int i;
	for(i=beg;i<=end;i++) {
		nodeArray[i].setNodeCaptureFile(path);
	}
	return SUCCESS;
}

int Config::setNodePosition(const string position, int beg, int end)
{
	int i;
	vector<string> pos=split(position, ',');
	if(pos.size() < 3) {
		ERROR << "Incorrect position supplied in config file\n";
		return FAILURE;
	}
	for(i=beg;i<=end;i++) {
		nodeArray[i].setNodePosition(stod(pos.at(0)), stod(pos.at(1)), stod(pos.at(2)));
	}
	return SUCCESS;
}

int Config::setConfigurationFromFile(const char *fname)
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
			if(!getline(is_line, key, '=')) continue;
			if(key[0] == '#') continue;
			if(!getline(is_line, value, '#')) continue;
			value = trim(value);
			key = trim(key);
			key = getKeyRange(key, beg_range, end_range);
			//INFO << "--- key=" << key << " beg=" << beg_range << " end=" << end_range << " val=" << value << endl;
			if(key == "numOfNodes") {
				setNumberOfNodes(stoi(value));
				set(key, value);
			} else if(key == "nodeExec") {
				setNodeSetExec(value, beg_range, end_range);
			} else if(key == "captureFile") {
				setNodeSetCapFile(value, beg_range, end_range);
			} else if(key == "nodePosition") {
				setNodePosition(value, beg_range, end_range);
			} else {
				set(key, value);
			}
		}
	} catch(exception & e) {
		ERROR << "Got exception " << e.what() << endl;
		return FAILURE;
	}
	return SUCCESS;
}

Nodeinfo *Config::get_node_info(uint16_t id) 
{
	if(!IN_RANGE(id, 0, getNumberOfNodes())) {
		return NULL;
	}
	return &nodeArray[id];
}

int Config::setNumberOfNodes(const int value)
{
	clearNodeArray();
	nodeArray = new Nodeinfo [value];
	numOfNodes = value;
	return SUCCESS;
}

void Config::clearNodeArray(void)
{
	if(nodeArray) {
		delete [] nodeArray;
		nodeArray = NULL;
		numOfNodes = 0;
	}
}
