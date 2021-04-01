#pragma once
#include "SFController.h"
#include <vector>
#include <mutex>
class SFProcessPool
{
public:
	std::vector<SFController*> pProcessList;
	SFProcessPool();
	
	~SFProcessPool();
	SFController* addProcess();
	SFController* getAvailableSFProcess();
	std::mutex sfMutex;
};
