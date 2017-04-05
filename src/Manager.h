#ifndef _MANAGER_H_
#define _MANAGER_H_

#include <common.h>
#include <Config.h>

class Manager
{
private:
  int startManager(wf::Config & cfg);

public:
  Manager() {};
  Manager(wf::Config & cfg);
};

#endif //_MANAGER_H_
