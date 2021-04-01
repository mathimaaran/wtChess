#pragma once
#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include "UI/WChessWidget.h"
#include "UI/SetFENPopup.h"
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WAnchor.h>
#include <Wt/WTimer.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WGridLayout.h>
#include <iostream>
#include <boost/thread.hpp>

#include "Service/SFController.h"
#include "Service/SFProcessPool.h"
#include <functional>

class ChessApplication : public Wt::WApplication
{
public:
	ChessApplication(const Wt::WEnvironment& env);

private:
	WChessWidget *chessboard_;
	Wt::WLineEdit *fen_;
	Wt::WTextArea *pgn_;
	SetFENPopup *popup_;
	void showSetFENPopup();
	void NewGameClicked();
	void heartbeat();
	void createLayout();
	//SFController sf;
};
