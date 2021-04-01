
#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WDialog.h>
#include <Wt/WJavaScript.h>
#include <Wt/WSignal.h>

class TrainingWidget : public Wt::WContainerWidget
{
private:
	
public:
	
	TrainingWidget(Wt::WApplication *);
	void loadFEN(const std::string& fen);
        void invalidFENProc(std::string errmsg);
        Wt::JSignal <std::string> *invalidFENSignal;
	Wt::JSignal <std::string> *updateFEN;
};
