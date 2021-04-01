#include "SFProcessPool.h"
#include <vector>
#include <boost/thread.hpp>

SFProcessPool::SFProcessPool()
{
	//2 SFControllers process at first
	for (int i = 0; i < 4; i++)
	{
		pProcessList.push_back(new SFController());
	}
	for (int i = 0; i < 4; i++)
	{
		boost::thread t(&SFController::startSF, pProcessList[i]);
                
	}
}
SFProcessPool::~SFProcessPool()
{
	std::lock_guard<std::mutex> guard(sfMutex);
	for (auto it = pProcessList.begin(); it != pProcessList.end(); it++)
	{
		delete *it;
	}
}

SFController* SFProcessPool::addProcess()
{
	//std::lock_guard<std::mutex> guard(sfMutex);
	pProcessList.push_back(new SFController());
	boost::thread t(&SFController::startSF, pProcessList[pProcessList.size() - 1]);
	return pProcessList[pProcessList.size() - 1];
}

SFController* SFProcessPool::getAvailableSFProcess()
{
	std::lock_guard<std::mutex> guard(sfMutex);
	auto it = std::find_if(pProcessList.begin(), pProcessList.end(), [](SFController* p)
	{
		return (!p->isBusy ? true : false);
	});
	if (it != pProcessList.end())
	{
		return *it;
	}
	else
	{
		return addProcess();
	}
}

