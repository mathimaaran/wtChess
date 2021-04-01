#include "SFController.h"

#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
#include <numeric>
SFController::SFController()
{
	isSFActive = false;
	isBusy = false;
}
SFController::~SFController()
{
	svc.stop();
}

void SFController::handle_read(const boost::system::error_code& error,
	size_t bytes_transferred)
{
	//isBusy = true;
	//std::cout << std::endl << "handling read" << std::endl;
	std::istream instream(&buf);
	std::string line;
	
	if (bytes_transferred == 0)
	{
		std::cout << "**** Handle READ bytes =0*******" << std::endl;
		return;
	}
	else
	{
		std::string lastLine;  // If last line is truncated , better retain it so that it gets processed in the next handle_read call.
		std::cout << "**** Transferred bytes *******" << bytes_transferred <<  std::endl;
		{//scope
			std::lock_guard<std::mutex> guard(dataMutex);
			bool firstRead = true;
			while (std::getline(instream, line,'\n')) 
			{
				if (line.length() <= 0) continue;

				if (instream.eof())
				{
					lastLine = line;
					dataFromEngine.push_back(line);
				}
				else
				{
					if (firstRead)
					{  //first record means we need to append the string to the last entry in the previous handle read call.
						firstRead = false;
						if (dataFromEngine.size() > 0)
							dataFromEngine[dataFromEngine.size() - 1].append(line);
						else
							dataFromEngine.push_back(line);
					}
					else
					{					
						dataFromEngine.push_back(line);
					}
				}
			}
		}
		std::cout << "Data From Engine Entries per read: " << dataFromEngine.size() << std::endl;
		auto x = boost::bind(&SFController::handle_read, this, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred);

		boost::asio::async_read_until(is, buf, std::string("\n"), x);
	        
		std::lock_guard<std::mutex> guard(dataMutex);
		// show contents of marked subexpressions within each match
		std::regex engine_error_regex("bestmove \\(none\\)");

		std::regex move_regex("bestmove ([a-h][1-8][a-h][1-8])");
		std::regex cp_score_regex ("score cp (-?[0-9]+)");
                
		std::smatch move_match, score_match;
		std::string bestmove;
		
		for (const auto& line : dataFromEngine) 
		{
			std::cout << "from Engine : " << line << std::endl;
			
			if (std::regex_search(line,move_match, engine_error_regex))
			{
				bestmove = "none";
				break;
			}
                        if (std::regex_search(line, score_match, cp_score_regex))
                        {
                            if (score_match.size() > 1)
                            {
                                std::string score = score_match[1];
                                float score_cp;
                                score_cp = boost::lexical_cast<int>(score)/100.0;
                                cpscorelist.push_back(score_cp);
                            }
                        }
			
			if (std::regex_search(line, move_match, move_regex)) 
			{
				std::cout << "matches for '" << line << "'" << std::endl;
				std::cout << "Prefix: '" << move_match.prefix() << "'" << std::endl;
				std::cout << "Suffix: '" << move_match.suffix() << "'" << std::endl;
				for (size_t i = 0; i < move_match.size(); ++i)
					std::cout << i << ": " << move_match[i] << std::endl;

				if (move_match.size() > 1)
				{
					/* TODO - Rare bug - Eg) when the read exactly cuts of at 'bestmove e7e8' */
					/* and if the subsequent read has 'n ponder f3f1' telling that the engine chose Knight as pawn promotion piece*/
					/* we lose the pawn promotion info and by default the pawn gets promoted as queen*/
					/* why can't we check for 'ponder' keyword also ?? Answer: when it is check mate ponder keyword doesn't come in SF output*/
					/* Why can't I check for presence of '\r' ?? Answer : Might not work in Linux*/
					bestmove = move_match[1];
					std::string promotion = "";
					if (move_match.suffix().str().length() > 0)
						if (move_match.suffix().str()[0] != ' ' && move_match.suffix().str()[0] != '\r' &&
							move_match.suffix().str()[0] != '\n')
							promotion = move_match.suffix().str()[0]; //assign char to a string
					bestmove = bestmove + promotion;
					break;
				}
				
			}
			
		}
		if (bestmove.length() > 1)
		{
                        float cpscore = (std::accumulate(cpscorelist.begin(), cpscorelist.end(), 0.0)) / cpscorelist.size();
			bestMoveSignal_.emit(bestmove);
			isBusy = false;
		}
		dataFromEngine.clear();
		dataFromEngine.push_back(lastLine);  //retain the last line

	}
	std::cout << "**** Handle READ complete*******" << std::endl;
}

void SFController::startSF() 
{

	
	std::vector<std::string> args;
	//args.push_back("--version");
        auto env = boost::this_process::environment();
        
	boost::filesystem::path p = env["STOCKFISH_PATH"].to_string();
	bp::child c(p, bp::std_out > is, bp::std_in < outstream);

	std::vector<std::string> data;
	std::string line;


	isSFActive = true;
        
        
	auto x = boost::bind(&SFController::handle_read, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred);

	boost::asio::async_read_until(is, buf, std::string("\n"), x);
        

	if (c.running())
	{
		//os << "quit\n";
	}
	try {
		svc.run();
	}
	catch (...)
	{
	}
 
	//c.wait();

	isSFActive = false;
	//return data;
		
}

bool SFController::sendCommand(std::string cmd)
{
	outstream << cmd;
	outstream.flush();
	return true;
}
std::string SFController::getFEN()
{
	return "";
}
bool SFController::setFEN(std::string)
{
	return "";
}
