#pragma once
#include <boost/process.hpp> 
#include <boost/asio.hpp>
#include <string> 
#include <vector> 
#include <iostream> 
#include <mutex>
#include <Wt/WSignal.h>
//#include <Wt/WIOService.h>

namespace bp = ::boost::process;
class SFController
{

public:
	bp::child process;
	bool isSFActive;
	bool isBusy;
	void startSF();
	
	SFController();
	~SFController();
	bool sendCommand(std::string);
	std::string getFEN();
	bool setFEN(std::string);
        
        
	boost::asio::io_service svc;
        //Wt::WIOService svc;
        

	bp::async_pipe is{ svc }, os{ svc };
        //bp::ipstream is;
	bp::opstream outstream;
	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred);
	boost::asio::streambuf buf;
	std::vector<std::string> dataFromEngine;
        std::vector<float> cpscorelist;
	std::mutex dataMutex;
	Wt::Signal<std::string> bestMoveSignal_;
};