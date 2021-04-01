#include "TrainingWidget.h"
#include <Wt/WMessageBox.h>
#include <Wt/WPushButton.h>
#include <iostream>

TrainingWidget::TrainingWidget(Wt::WApplication* theApp) : WContainerWidget()
{

	updateFEN = new Wt::JSignal<std::string>(this, "updateFENSignal");

        invalidFENSignal = new Wt::JSignal<std::string>(this, "InvalidFENSignal");
	invalidFENSignal->connect(this, &TrainingWidget::invalidFENProc);
        
	std::ostringstream osjscb;
	
	osjscb << "window.trainingboard_" << id() << " = null" << std::endl
		<< "window.traininggame_" << id() << " = new Chess()" << std::endl
		//<< "window.game_" << id() << ".load ('2r2rk1/pp1b1ppp/6q1/3p4/N2b4/PP1PnP2/3KQ1PP/R3B2R w - - 12 23')" << std::endl
		<< "var $status = ''" << std::endl
		<< "var $fen = ''" << std::endl
		<< "var $pgn = ''" << std::endl

		// update the board position after the piece snap
		// for castling, en passant, pawn promotion
		<< "function onChange (oldPos, newPos) {" << std::endl
		//<< "    console.log (window.traininggame_" << id() << ".fen())" << std::endl
		<< "		window.trainingupdateStatus(Chessboard.objToFen(newPos))" << std::endl
		<< "}" << std::endl
		<< std::endl

		<< "window.trainingupdateStatus = function (newfen) {" << std::endl

		//<< updateFEN->createCall({ "window.trainingboard_" + id() + ".fen()" }) << std::endl
		<< updateFEN->createCall({ "newfen" }) << std::endl
		//<< "	$status.html(status)" << std::endl
		//<< "		$fen.html(game.fen())" << std::endl
		//<< "		$pgn.html(game.pgn())" << std::endl
		<< "}" << std::endl
		<< std::endl
		<< "var config_" << id() << " = {" << std::endl
		<< "  draggable: true, " << std::endl
		<< "  position : 'start'," << std::endl
		<< "  dropOffBoard : 'trash', " << std::endl
		<< "  sparePieces : true, " << std::endl
		<< "  onChange : onChange," << std::endl
		<< "}" << std::endl
		<< "	window.trainingboard_" << id() << " = Chessboard('" << id() << "', config_" << id() << ")" << std::endl
		//<< " window.board_" << id() << ".position('2r2rk1/pp1b1ppp/6q1/3p4/N2b4/PP1PnP2/3KQ1PP/R3B2R w - - 12 23')" << std::endl
		<< "	window.trainingupdateStatus(window.trainingboard_" << id() << ".fen())" << std::endl;

	doJavaScript(osjscb.str());
}
void TrainingWidget::loadFEN(const std::string& fen)
{
    
    std::ostringstream osloadFEN;
    osloadFEN << "var obj = window.trainingboard_" << id() << ".position(" << Wt::WString(fen).jsStringLiteral() << ")" << std::endl;
	//osloadFEN << "console.log(JSON.stringify(obj))" << std::endl;
        osloadFEN << " var tempChess = new Chess()" << std::endl;
        osloadFEN << " var fenstatus = tempChess.validate_fen(" << Wt::WString(fen).jsStringLiteral() << ")" << std::endl;
        osloadFEN << "  if (fenstatus.valid === false) {" << std::endl;
        osloadFEN << "       " << invalidFENSignal->createCall({ "fenstatus.error" }) << std::endl;
        osloadFEN << "    tempChess = null" << std::endl;
        osloadFEN << "    return" << std::endl;
        osloadFEN << " }" << std::endl;
	doJavaScript(osloadFEN.str());
}
void TrainingWidget::invalidFENProc(std::string errmsg)
{
	Wt::WMessageBox::show("Chess - Error", std::string("Invalid FEN : " + errmsg),
		Wt::StandardButton::Ok);

}