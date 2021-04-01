#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WDialog.h>
#include <Wt/WJavaScript.h>
#include <Wt/WSignal.h>
#include "../Service/SFController.h"
#include "../Service/SFProcessPool.h"
#include <mutex>
enum class PieceColor { WHITE, BLACK };

class PromotionPopup : public Wt::WObject
{
		
public:
	Wt::Signal<std::string> promotionselected_;
	Wt::WDialog *dialog_;
	PromotionPopup(PieceColor color);
	~PromotionPopup();
	void dialogDone(Wt::DialogCode code);
	PieceColor color_;
	std::string selectedPiece;
	void pieceSelect(int index, Wt::WImage * img);
};

class WChessWidget : public Wt::WContainerWidget
{
private:

public:

	WChessWidget(SFProcessPool*,  Wt::WApplication*);
	Wt::JSignal < std::string> *moveMade;
	Wt::JSignal < std::string> *gameOver;
	Wt::JSignal < std::string, std::string, std::string> *pawnpromotion;
	Wt::JSignal < std::string> *personMoveFEN;
	Wt::JSignal <std::string, std::string> *updateFENPGN;
	Wt::JSignal <std::string> *invalidFENSignal;
	void moveMadeProc(std::string);
	void gameOverProc(std::string);
	void pawnPromotionPopup(std::string color, std::string from, std::string to);
	void addPersonMoveFEN(std::string);
	void invalidFENProc(std::string errmsg);
	void makeEngineMove(std::string move);
	void undoMove(bool newGame=false, std::string fen = "");
	void newGame(std::string fen= "");
	Wt::WApplication *ptrApp;
	SFProcessPool* _sfpp;
	std::vector<std::string> personMoveFENs;
	std::string personMoveJSCall;
	std::mutex personMoveDataMutex;
	Wt::Signal<std::string> disableUndoSignal_;
	Wt::Signals::connection sfConn_;
	PieceColor color;
	std::string lastSetFEN_;
	PromotionPopup *popup_;
};