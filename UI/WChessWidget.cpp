#include "WChessWidget.h"
#include <Wt/WMessageBox.h>
#include <Wt/WPushButton.h>
#include <iostream>

WChessWidget::WChessWidget(SFProcessPool* sfpp, Wt::WApplication* theApp): WContainerWidget()
{
	_sfpp = sfpp;
	ptrApp = theApp;
	color = PieceColor::WHITE;
	lastSetFEN_ = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	personMoveFENs.push_back("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	//_sf->bestMoveSignal_.connect(this, &WChessWidget::makeEngineMove);
	moveMade = new Wt::JSignal<std::string>(this, "moveSignal");
	moveMade->connect(this, &WChessWidget::moveMadeProc);
	gameOver = new Wt::JSignal<std::string>(this, "gameOverSignal");
	gameOver->connect(this, &WChessWidget::gameOverProc);
	pawnpromotion = new Wt::JSignal<std::string, std::string, std::string>(this, "PawnPromotion");
	pawnpromotion->connect(this, &WChessWidget::pawnPromotionPopup);

	updateFENPGN = new Wt::JSignal<std::string,std::string>(this, "updateFENPGNSignal");
	
	personMoveFEN = new Wt::JSignal<std::string>(this, "personMoveSignal");
	personMoveFEN->connect(this, &WChessWidget::addPersonMoveFEN);
	personMoveJSCall = personMoveFEN->createCall({ "window.game_" + id() + ".fen()" }); //Hack!?

	invalidFENSignal = new Wt::JSignal<std::string>(this, "InvalidFENSignal");
	invalidFENSignal->connect(this, &WChessWidget::invalidFENProc);

	std::ostringstream osjscb;
	/*osjscb << "var config_" << id() << " = {" << std::endl
		<< "draggable : true, " << std::endl
		<< "dropOffBoard : 'snapback', " << std::endl
		<< "position : 'start'" << std::endl
		<< "}" << std::endl << std::endl;
	osjscb << "var board = Chessboard('" << id() << "', config_" << id() << ")" << std::endl;*/

	osjscb << "window.board_" << id() << " = null" << std::endl
		<< "window.game_" << id() << " = new Chess()" << std::endl
		//<< "window.game_" << id() << ".load ('2r2rk1/pp1b1ppp/6q1/3p4/N2b4/PP1PnP2/3KQ1PP/R3B2R w - - 12 23')" << std::endl
		<< "var $status = ''" << std::endl
		<< "var $fen = ''" << std::endl
		<< "var $pgn = ''" << std::endl
		<< "var whiteSquareGrey = '#a9a9a9'" << std::endl
		<< "var blackSquareGrey = '#696969'" << std::endl
        << " window.squareClass = 'square-55d63'" << std::endl
	    << " window.squareToHighlight = null" << std::endl
	    << " window.colorToHighlight = null" << std::endl

		<< std::endl
		<< "function removeGreySquares() {" << std::endl
		<< "  $('#" << id() << " .square-55d63').css('background', '')" << std::endl
		<< "}" << std::endl
		<< "function greySquare (square) {" << std::endl
		<< "  var $square = $('#" << id() << " .square-' + square)" << std::endl
		<< "  var background = whiteSquareGrey" << std::endl
		<< "  if ($square.hasClass('black-3c85d')) {" << std::endl
		<< "	background = blackSquareGrey" << std::endl
		<< "  }" << std::endl

		<< "  $square.css('background', background)" << std::endl
		<< "}" << std::endl



		<< std::endl
		<< "function onDragStart(source, piece, position, orientation) {" << std::endl
		// do not pick up pieces if the game is over
		<< "  if (window.game_" << id() << ".game_over()) return false" << std::endl
		<< std::endl
		// only pick up pieces for human player to move
		<< "  var humanplayercolor = orientation.substring(0,1)" << std::endl
		//<< "  if ((window.game_" << id() << ".turn() === 'w' && piece.search(/ ^b / ) !== -1) || \
				//(window.game_" << id() << ".turn() === 'b' && piece.search(/ ^w / ) !== -1)) { 
				//<< "    console.log('human player color ' + humanplayercolor)" << std::endl
				//<< "    console.log('turn : ' + window.game_" << id() << ".turn())" << std::endl
		<< "  if (window.game_" << id() << ".turn() !== humanplayercolor){" << std::endl
		//<< "    console.log('disabling move when computer is thinking')" << std::endl
		<< "        return false " << std::endl
		<< "	}" << std::endl
		<< "}" << std::endl
		<< std::endl
		<< "function onDrop(source, target) {" << std::endl
		<< "removeGreySquares()" << std::endl
		// see if the move is legal
		<< "	var move = window.game_" << id() << ".move({" << std::endl
		<< "	  from: source," << std::endl
		<< "	  to : target," << std::endl
		<< "	  promotion : 'q'" << std::endl // NOTE: always promote to a queen for example simplicity
		<< "		})" << std::endl
		<< std::endl
		// illegal move
		<< "		if (move === null) return 'snapback'" << std::endl
		<< " if (move.flags.includes('p')) { " << std::endl
		<< pawnpromotion->createCall({ "move.color", "source", "target" }) << std::endl
		<< " window.game_" << id() << ".undo()" << std::endl  //halfmove
		<< "    return; " << std::endl
		<< "}" << std::endl
		<< std::endl
		
		<< " if (move.color === 'w') {" << std::endl
	    << " $('#" << id() << "').find('.' + window.squareClass).removeClass('highlight-white')" << std::endl
	    << " 	$('#" << id() << "').find('.square-' + move.from).addClass('highlight-white')" << std::endl
	    << " 	window.squareToHighlight = move.to" << std::endl
	    << " 	window.colorToHighlight = 'white'" << std::endl
	    << " }" << std::endl
	    << " else {" << std::endl
	    << " $('#" << id() << "').find('.' + window.squareClass).removeClass('highlight-black')" << std::endl
	    << " $('#" << id() << "').find('.square-' + move.from).addClass('highlight-black')" << std::endl
	    << " window.squareToHighlight = move.to" << std::endl
	    << " window.colorToHighlight = 'black'" << std::endl
	    << " }" << std::endl
		// onMoveEnd() not called after this!? so hightlight forcefully
		<< "  $('#" << id() << "').find('.square-' + window.squareToHighlight)"
		<< "  .addClass('highlight-' + window.colorToHighlight)" << std::endl

		<< moveMade->createCall({ "window.game_" + id() + ".fen()" }) << std::endl
		<< "		window.updateStatus()" << std::endl
		<< "}" << std::endl
		<< std::endl
		<< "function onMouseoverSquare(square, piece) {" << std::endl
		// get list of possible moves for this square
		<< "var moves = window.game_" + id() + ".moves({" << std::endl
		<< "  square: square," << std::endl
		<< "  verbose : true" << std::endl
		<< "	})" << std::endl
		<< std::endl
		// exit if there are no moves available for this square
		<< "	if (moves.length === 0) return" << std::endl

		// highlight the square they moused over
		<< "		greySquare(square)" << std::endl

		// highlight the possible squares for this piece
		<< "		for (var i = 0; i < moves.length; i++) {" << std::endl
		<< "			greySquare(moves[i].to)" << std::endl
		<< "		}" << std::endl
		<< "}" << std::endl
		<< std::endl
		<< "function onMouseoutSquare(square, piece) {" << std::endl
		<< "removeGreySquares()" << std::endl
		<< "}" << std::endl

		// update the board position after the piece snap
		// for castling, en passant, pawn promotion
		<< "function onSnapEnd() {" << std::endl
		<< "  	window.board_" << id() << ".position(window.game_" << id() << ".fen())" << std::endl
		<< "    console.log (window.game_" << id() << ".fen())" << std::endl
		<< "		window.updateStatus()" << std::endl
		<< "}" << std::endl
		<< std::endl
		<< "function onMoveEnd() {" << std::endl
		<< "  $('#" << id() << "').find('.square-' + window.squareToHighlight)" 
		<< "  .addClass('highlight-' + window.colorToHighlight)" << std::endl
		<< "}"
		<< std::endl
		<< "window.updateStatus = function () {" << std::endl
		<< "	var status = ''" << std::endl
		<< std::endl
		<< "		var moveColor = 'White'" << std::endl
		<< "		if (window.game_" << id() << ".turn() === 'b') {" << std::endl
		<< "			moveColor = 'Black'" << std::endl
		<< "		}" << std::endl
		<< std::endl
		// checkmate?
		<< "	if (window.game_" << id() << ".in_checkmate()) {" << std::endl
		<< "		status = 'Game over, ' + moveColor + ' is in checkmate.'" << std::endl
		<< gameOver->createCall({ "status" }) << std::endl
		<< "	}" << std::endl
		<< std::endl
		// draw?
		<< "	else if (window.game_" << id() << ".in_draw()) {" << std::endl
		<< "		status = 'Game over, drawn position'" << std::endl
		<< gameOver->createCall({ "status" }) << std::endl
		<< "	}" << std::endl
		<< std::endl
		// game still on
		<< "	else {" << std::endl
		<< "		status = moveColor + ' to move'" << std::endl
		<< std::endl
		// check?
		<< "			if (window.game_" << id() << ".in_check()) {" << std::endl
		<< "				status += ', ' + moveColor + ' is in check'" << std::endl
		<< "			}" << std::endl
		<< "	}" << std::endl
		<< std::endl
		<< updateFENPGN->createCall({ "window.game_"+id()+".fen()", "window.game_"+id()+".pgn()" }) << std::endl
		//<< "	$status.html(status)" << std::endl
		//<< "		$fen.html(game.fen())" << std::endl
		//<< "		$pgn.html(game.pgn())" << std::endl
		<< "}" << std::endl
		<< std::endl
		<< "var config_" << id() << " = {" << std::endl
		<< "  draggable: true, " << std::endl
		<< "  position : 'start'," << std::endl
		<< "  onDragStart : onDragStart," << std::endl
		<< "  onDrop : onDrop," << std::endl
		<< "  onMouseoutSquare : onMouseoutSquare," << std::endl
		<< "  onMouseoverSquare : onMouseoverSquare," << std::endl
		<< "  onSnapEnd : onSnapEnd," << std::endl
		<< "  onMoveEnd : onMoveEnd" << std::endl
		<< "}" << std::endl
		<< "	window.board_" << id() << " = Chessboard('" << id() << "', config_" << id() << ")" << std::endl
		//<< " window.board_" << id() << ".position('2r2rk1/pp1b1ppp/6q1/3p4/N2b4/PP1PnP2/3KQ1PP/R3B2R w - - 12 23')" << std::endl
		<< "	window.updateStatus()" << std::endl;

	doJavaScript(osjscb.str());
}
void WChessWidget::makeEngineMove(std::string move)
{
	if (move == "none")
	{
		Wt::WApplication::UpdateLock errorlock(ptrApp);
		if (errorlock) {
			Wt::WMessageBox::show("Chess - Error", std::string("Chess Engine - Error"),
				Wt::StandardButton::Ok);
			disableUndoSignal_.emit("enable");
		}
		ptrApp->triggerUpdate();
		return;
	}
	if (sfConn_.isConnected())
	{
		sfConn_.disconnect();
	}
	std::string from = move.substr(0, 2);
	std::string to = move.substr(2, 2);
	std::string promotion = (move.length() == 5) ? move.substr(4, 1) : "q";
	std::ostringstream osmove;
	
	osmove << "var enginemove = window.game_" << id() << ".move({ from: '" + from + "', to: '" + to + "', promotion : '" + promotion +"' })" << std::endl;

	osmove << " if (enginemove.color === 'w') {" << std::endl;
	osmove << " $('#" << id() << "').find('.' + window.squareClass).removeClass('highlight-white')" << std::endl;
	osmove << " 	$('#" << id() << "').find('.square-' + enginemove.from).addClass('highlight-white')" << std::endl;
	osmove << " 	window.squareToHighlight = enginemove.to" << std::endl;
	osmove << " 	window.colorToHighlight = 'white'" << std::endl;
	osmove << " }" << std::endl;
	osmove << " else {" << std::endl;
	osmove << " $('#" << id() << "').find('.' + window.squareClass).removeClass('highlight-black')" << std::endl;
	osmove << " $('#" << id() << "').find('.square-' + enginemove.from).addClass('highlight-black')" << std::endl;
	osmove << " window.squareToHighlight = enginemove.to" << std::endl;
	osmove << " window.colorToHighlight = 'black'" << std::endl;
	osmove << " }" << std::endl;

	osmove << " window.board_" << id() << ".move('" + from + "-" + to + "')" << std::endl;

	//below line might be overkill but to fix pawn promotion , castling and en passant issue
	osmove << " window.board_" << id() << ".position(window.game_" << id() << ".fen())" << std::endl;
	osmove << "	window.updateStatus()" << std::endl;
	//osmove << "	if (!window.game_" << id() << ".in_checkmate() && !window.game_" << id() << ".in_draw()) {" << std::endl;
	osmove << personMoveJSCall << std::endl;
	//osmove << "	}" << std::endl;
	
	//std::cout << " ##############" << std::endl;
	//std::cout << osmove.str().c_str() << std::endl;
	//std::cout << " ##############" << std::endl;
	Wt::WApplication::UpdateLock lock(ptrApp);
	if (lock) {
		doJavaScript(osmove.str());
		disableUndoSignal_.emit("enable");
	}
	ptrApp->triggerUpdate();
	
}
void WChessWidget::undoMove(bool newGame, std::string strfen)
{
	std::lock_guard<std::mutex> guard(personMoveDataMutex);
	bool initposition = false;
	Wt::StandardButton answer = Wt::StandardButton::Yes;
        PieceColor whoseturn = PieceColor::WHITE;
	if (newGame)
	{
		//if (strfen.length() == 0)
		{
			answer = Wt::WMessageBox::show("Chess", std::string("Do you want to play with White pieces?"),
				Wt::StandardButton::Yes | Wt::StandardButton::No | Wt::StandardButton::Cancel);
		}
		initposition = false;
		if (answer == Wt::StandardButton::Cancel)
			return;

		if (strfen.length() == 0)
		{
			personMoveFENs.clear();
			personMoveFENs.push_back("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); 
			//personMoveFENs.push_back("8/8/8/8/2K5/8/6pk/8 w - - 1 1");
                        whoseturn = PieceColor::WHITE;
		}
		else
		{
                    std::string toMove;
			personMoveFENs.clear();
			personMoveFENs.push_back(strfen);
                        
                        std::vector<std::string> result; 
                        std::istringstream iss(strfen); 
                        for(std::string s; iss >> s; ) 
                            result.push_back(s); 
                        if (result.size() > 1)
                        {
                            toMove = result[1];
                            whoseturn =  ((toMove == "w") ? PieceColor::WHITE : PieceColor::BLACK);
                        }
                        
		}
	}
	
	
	if (!newGame && personMoveFENs.size() > 1)
		personMoveFENs.pop_back();
	if (personMoveFENs.size() == 1 /*&& strfen.length() == 0*/)
	{
		initposition = true;
	}
        
        //if (answer == Wt::StandardButton::Yes)
        //    blackInit = false;
        
	std::string targetFEN = personMoveFENs.back();
	

	std::ostringstream osum;
	
        if (newGame)
        {
			osum << " var tempChess = new Chess()" << std::endl;
			osum << " var fenstatus = tempChess.validate_fen(" << Wt::WString(targetFEN).jsStringLiteral() << ")" << std::endl;
			osum << "  if (fenstatus.valid === false) {" << std::endl;
			osum << "       " << invalidFENSignal->createCall({ "fenstatus.error" }) << std::endl;
			osum << "    tempChess = null" << std::endl;
			osum << "    return" << std::endl;
			osum << " }" << std::endl;
			osum << "var gamestatus = tempChess.load('" + targetFEN + "')" << std::endl;
			osum << " if(gamestatus === false || tempChess.game_over() === true) {" << std::endl;
			osum << "       " << invalidFENSignal->createCall({ "'Unable to start game. Game Over Scenario!'" }) << std::endl;
			osum << "    tempChess = null" << std::endl;
			osum << "    return" << std::endl;
			osum << " } " << std::endl;
			osum << " window.game_" << id() << ".load('" + targetFEN + "')" << std::endl;
        }
        else
        {
            //osum << "console.log( window.game_" << id() << ".pgn())" << std::endl;
                    
            osum << " window.game_" << id() << ".undo()" << std::endl;  //halfmove
            osum << " window.game_" << id() << ".undo()" << std::endl;  //halfmove
            //osum << "console.log( window.game_" << id() << ".pgn())" << std::endl;
        }
	osum << " window.board_" << id() << ".position(window.game_" << id() << ".fen())" << std::endl;
	if (/*strfen.length() == 0 && */(newGame))
	{
		if (answer == Wt::StandardButton::No)
		{
			osum << " window.board_" << id() << ".orientation('black')" << std::endl;
			color = PieceColor::BLACK;

		}
		else
		{
			color = PieceColor::WHITE;
			osum << " window.board_" << id() << ".orientation('white')" << std::endl;
		}
                if (whoseturn != color)
                    moveMadeProc(targetFEN);
		
	}
        else if (initposition)
        {
            std::vector<std::string> result; 
            std::istringstream iss(targetFEN); 
            for(std::string s; iss >> s; ) 
                result.push_back(s); 
            if (result.size() > 1)
            {
                whoseturn =  ((result[1] == "w") ? PieceColor::WHITE : PieceColor::BLACK);
            }
            if (whoseturn != color)
                moveMadeProc(targetFEN);
        }

	//Remove all Highlights
	osum << " $('#" << id() << "').find('.' + window.squareClass).removeClass('highlight-white')" << std::endl;
	osum << " $('#" << id() << "').find('.' + window.squareClass).removeClass('highlight-black')" << std::endl;
	osum << " window.squareToHighlight = null" << std::endl;
	osum << " window.colorToHighlight = null" << std::endl;

	if (!newGame)
	{
		osum << " var movehistory = window.game_" << id() << ".history({verbose : true})" << std::endl;

		osum << " if (movehistory.length > 0) {" << std::endl;
		osum << "    for (const item of movehistory) {" << std::endl;
		osum << " 	     console.log('{' + item.color + ',' + item.from + ',' + item.to + '}')" << std::endl;
		osum << "    }" << std::endl;
		osum << " }" << std::endl;
		osum << " var lastMove1 = null" << std::endl;
		osum << " if (movehistory.length > 0) {" << std::endl;
		osum << "       lastMove1 = movehistory[movehistory.length - 1]" << std::endl;
		osum << " }" << std::endl;
		osum << " var lastMove1 = movehistory[movehistory.length-1]" << std::endl;
		osum << " var lastMove2 = null" << std::endl;
		osum << " if (movehistory.length > 1) {" << std::endl;
		osum << "       lastMove2 = movehistory[movehistory.length - 2]" << std::endl;
		osum << " }" << std::endl;

		osum << " if (lastMove1 !== null && lastMove1 !== undefined){" << std::endl;
		osum << "     $('#" << id() << "').find('.square-' + lastMove1.from).addClass((lastMove1.color === 'w') ? 'highlight-white' : 'highlight-black')" << std::endl;
		osum << "     $('#" << id() << "').find('.square-' + lastMove1.to).addClass((lastMove1.color === 'w') ? 'highlight-white' : 'highlight-black')" << std::endl;
		osum << "}" << std::endl;
		osum << " if (lastMove2 !== null && lastMove2 !== undefined){" << std::endl;
		osum << "    $('#" << id() << "').find('.square-' + lastMove2.from).addClass((lastMove2.color === 'w') ? 'highlight-white' : 'highlight-black')" << std::endl;
		osum << "    $('#" << id() << "').find('.square-' + lastMove2.to).addClass((lastMove2.color === 'w') ? 'highlight-white' : 'highlight-black')" << std::endl;
		osum << "}" << std::endl;
	}

	osum << " window.updateStatus()" << std::endl;
	doJavaScript(osum.str());
	
}
void WChessWidget::newGame(std::string fen)
{
	
	undoMove(true, fen); //Passing true means for newGame (to ask user, Black or White?)
}
void WChessWidget::pawnPromotionPopup(std::string strcolor, std::string from, std::string to)
{
	popup_ = new PromotionPopup(strcolor == "w" ? PieceColor::WHITE : PieceColor::BLACK);
	popup_->promotionselected_.connect([=](std::string promotion) {
		char piece = 'q';
		if (promotion.length() > 1)
			piece = promotion[1];
		piece = tolower(piece);
		std::ostringstream ospromote;
		ospromote 
			<< "	var move = window.game_" << id() << ".move({" << std::endl
			<< "	  from: '" << from << "'," << std::endl
			<< "	  to : '" << to << "'," << std::endl
			<< "	  promotion : '"<< piece << "'" << std::endl // NOTE: always promote to a queen for example simplicity
			<< "		})" << std::endl
			
			<< " if (move.color === 'w') {" << std::endl
			<< " $('#" << id() << "').find('.' + window.squareClass).removeClass('highlight-white')" << std::endl
			<< " 	$('#" << id() << "').find('.square-' + move.from).addClass('highlight-white')" << std::endl
			<< " 	window.squareToHighlight = move.to" << std::endl
			<< " 	window.colorToHighlight = 'white'" << std::endl
			<< " }" << std::endl
			<< " else {" << std::endl
			<< " $('#" << id() << "').find('.' + window.squareClass).removeClass('highlight-black')" << std::endl
			<< " $('#" << id() << "').find('.square-' + move.from).addClass('highlight-black')" << std::endl
			<< " window.squareToHighlight = move.to" << std::endl
			<< " window.colorToHighlight = 'black'" << std::endl
			<< " }" << std::endl
			// onMoveEnd() not called after this!? so hightlight forcefully
			<< "  $('#" << id() << "').find('.square-' + window.squareToHighlight)"
			<< "  .addClass('highlight-' + window.colorToHighlight)" << std::endl

			<< moveMade->createCall({ "window.game_" + id() + ".fen()" }) << std::endl
			<< " window.board_" << id() << ".position(window.game_" << id() << ".fen())" << std::endl
			<< "		window.updateStatus()" << std::endl
			//<< "}" << std::endl
			<< std::endl;

		doJavaScript(ospromote.str());
	});
}
void WChessWidget::moveMadeProc(std::string fen)
{
	disableUndoSignal_.emit("disable");
	SFController* sfc = _sfpp->getAvailableSFProcess();
	sfc->sendCommand("position fen " + fen + "\n");
	//sfc->sendCommand("setoption name MultiPV value 3\n");
	sfc->sendCommand("go movetime 2000\n");
	sfc->isBusy = true;
        sfc->cpscorelist.clear();
	sfConn_ = sfc->bestMoveSignal_.connect(this, &WChessWidget::makeEngineMove);
        
}

void WChessWidget::gameOverProc(std::string status)
{
	Wt::StandardButton answer
		= Wt::WMessageBox::show("Chess",status,
			Wt::StandardButton::Ok);
}

void WChessWidget::addPersonMoveFEN(std::string FEN)
{
	std::lock_guard<std::mutex> guard(personMoveDataMutex);
	personMoveFENs.push_back(FEN);
}

void WChessWidget::invalidFENProc(std::string errmsg)
{
	Wt::WMessageBox::show("Chess - Error", std::string("Invalid FEN : " + errmsg),
		Wt::StandardButton::Ok);

}
PromotionPopup::~PromotionPopup()
{
	// delete dialog_;
}
PromotionPopup::PromotionPopup(PieceColor color) : WObject()
{
	color_ = color;
	selectedPiece = (color_ == PieceColor::WHITE) ? 'w' : 'b';
	selectedPiece += "Q";
	dialog_ = addChild(Wt::cpp14::make_unique<Wt::WDialog>("Choose Promotion Piece"));
	std::string strColor = (color_ == PieceColor::WHITE) ? "w" : "b";
	std::vector<std::string> imagelinkList;
	imagelinkList.push_back("img/chesspieces/wikipedia/" + strColor + "Q.png");
	imagelinkList.push_back("img/chesspieces/wikipedia/" + strColor + "R.png");
	imagelinkList.push_back("img/chesspieces/wikipedia/" + strColor + "B.png");
	imagelinkList.push_back("img/chesspieces/wikipedia/" + strColor + "N.png");
	for (int ii = 0; ii < imagelinkList.size(); ii++)
	{
		Wt::WImage* img = dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WImage>(imagelinkList[ii]));
		img->clicked().connect([=]() {
			pieceSelect(ii, img);
		});
		if (ii == 0)
			img->decorationStyle().setBorder(Wt::WBorder(Wt::BorderStyle::Solid, 2));
	}
	dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WBreak>());
	auto okButton = dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "OK"));
	okButton->setStyleClass("waves-effect brown waves-light btn-small z-depth-2");
	okButton->clicked().connect(dialog_, &Wt::WDialog::accept);

	dialog_->finished().connect(this, &PromotionPopup::dialogDone);
	dialog_->show();
}
void PromotionPopup::dialogDone(Wt::DialogCode code)
{
	promotionselected_.emit(selectedPiece);
	removeChild(dialog_);
}
void PromotionPopup::pieceSelect(int index, Wt::WImage * img)
{

	for (int ii = 0; ii < dialog_->contents()->count(); ii++)
	{
		try
		{
			Wt::WImage *ptr = dynamic_cast<Wt::WImage*>(dialog_->contents()->widget(ii));
			if (ptr != nullptr)
				ptr->decorationStyle().setBorder(Wt::WBorder(Wt::BorderStyle::None, 0));
		}
		catch (std::bad_cast& ex)
		{
		}
	}
	img->decorationStyle().setBorder(Wt::WBorder(Wt::BorderStyle::Solid, 2));
	selectedPiece = (color_ == PieceColor::WHITE) ? 'w' : 'b';
	if (index > 3) 
		index = 0;
	selectedPiece += std::string("QRBN").at(index);

}
