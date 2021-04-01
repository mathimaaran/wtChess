/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "main.h"

SFProcessPool masterPool;

/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
ChessApplication::ChessApplication(const Wt::WEnvironment& env)
  : WApplication(env)
{
	WApplication::enableUpdates();
	popup_ = nullptr;
	//setCssTheme("");
	useStyleSheet("https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css");
	useStyleSheet("https://fonts.googleapis.com/icon?family=Material+Icons");
    useStyleSheet("https://allfont.net/allfont.css?fonts=agency-fb");
	useStyleSheet("resources/chessboard/css/chessboard-1.0.0.css");
	useStyleSheet("css/Parikart.css");

	require("https://code.jquery.com/jquery-3.5.1.min.js");
	require("https://unpkg.com/@chrisoakman/chessboardjs@1.0.0/dist/chessboard-1.0.0.min.js");
	require("https://cdnjs.cloudflare.com/ajax/libs/chess.js/0.9.3/chess.min.js");
	require("https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/js/materialize.min.js");
	

    setTitle("Chess Wrapper");                            // application title
  
  
  root()->setStyleClass("container");

  createLayout();

    
  auto timer = root()->addChild(Wt::cpp14::make_unique<Wt::WTimer>());
  timer->setInterval(std::chrono::seconds(60));
  timer->timeout().connect(this, &ChessApplication::heartbeat);
  timer->start();
}
void ChessApplication::heartbeat()
{
	std::cout << "************heart beat*******" << std::endl;
}
void ChessApplication::NewGameClicked()
{
  /*
   * Update the text, using text input into the nameEdit_ field.
   */
	
	chessboard_->newGame();
  
}

void ChessApplication::showSetFENPopup()
{
	if (popup_ != nullptr)
		delete popup_;
	popup_ = new SetFENPopup(chessboard_->lastSetFEN_, this);
	popup_->fenselected_.connect([&](std::string strfen) {
		chessboard_->lastSetFEN_ = strfen;
		chessboard_->newGame(strfen);
	});
}
int main(int argc, char **argv)
{
  /*
   * Your main method may set up some shared resources, but should then
   * start the server application (FastCGI or httpd) that starts listening
   * for requests, and handles all of the application life cycles.
   *
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */
  return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
    /*
     * You could read information from the environment to decide whether
     * the user has permission to start a new application
     */
    return Wt::cpp14::make_unique<ChessApplication>(env);
  });
}

void ChessApplication::createLayout()
{
	//root()->addWidget(Wt::cpp14::make_unique<Wt::WBreak>());    // insert a line break


	auto newGameButton = root()->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "New Game"));
	auto sportsicon_ = newGameButton->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "sports"));
	sportsicon_->setHtmlTagName("i");
	sportsicon_->setStyleClass("material-icons left");
	newGameButton->setStyleClass("waves-effect brown waves-light btn-small z-depth-2");
        
        root()->addWidget(Wt::cpp14::make_unique<Wt::WText>(" .......... "));
        
        Wt::WImage *image = root()->addNew<Wt::WImage>(Wt::WLink("img/Stockfish11.png"));
        image->setAlternateText("Stockfish Image");
        image->resize(64,64);
        
        
	//root()->addWidget(Wt::cpp14::make_unique<Wt::WBreak>());    // insert a line break
      //<p style="font-family:'Agency FB';">STOCKFISH 11</p>
        Wt::WText *sf11 = root()->addWidget(Wt::cpp14::make_unique<Wt::WText>("<span style='font-family:Agency FB; font-size: 14pt'>STOCKFISH 11</span>"));
        
        Wt::WText *lastUpdated = root()->addNew<Wt::WText>(std::string("Last Compiled :: ") + __DATE__ + " " + __TIME__);
        lastUpdated->setAttributeValue("style","float:right");
	auto gamecontainer = root()->addWidget(Wt::cpp14::make_unique<Wt::WContainerWidget>());

	auto mainhbox = gamecontainer->setLayout(Wt::cpp14::make_unique<Wt::WHBoxLayout>());

	auto chessboardUptr = Wt::cpp14::make_unique<WChessWidget>(&masterPool, this);
	chessboard_ = mainhbox->addWidget(std::move(chessboardUptr));
	chessboard_->setWidth(Wt::WLength(30, Wt::LengthUnit::Percentage));
	//chessboard_->setHeight(Wt::WLength(80, Wt::LengthUnit::Percentage));
	
	auto griddata = Wt::cpp14::make_unique < Wt::WGridLayout>();
	griddata->addWidget(Wt::cpp14::make_unique<Wt::WText>("FEN: "),0,0);
	fen_ = griddata->addWidget(Wt::cpp14::make_unique<Wt::WLineEdit>(),0,1);
	fen_->setReadOnly(true);
	//fen_->setAttributeValue("style", "border : 2px");

	griddata->addWidget(Wt::cpp14::make_unique<Wt::WText>("PGN: "), 1, 0);
	pgn_ = griddata->addWidget(Wt::cpp14::make_unique<Wt::WTextArea>(), 1, 1);
	pgn_->setAttributeValue("style", "resize:none");
	pgn_->setReadOnly(true);
	
	chessboard_->updateFENPGN->connect([=](std::string fen, std::string pgn) {
		fen_->setText(fen);
		pgn_->setText(pgn);
	});
	//griddata->addWidget(Wt::cpp14::make_unique<Wt::WText>("SF Log: "), 2, 0);
	//griddata->addWidget(Wt::cpp14::make_unique<Wt::WLineEdit>(), 2, 1);
	griddata->addWidget(Wt::cpp14::make_unique<Wt::WText>(" "), 2, 0);
	griddata->addWidget(Wt::cpp14::make_unique<Wt::WText>(" "), 2, 1);
	griddata->setRowStretch(2, 1);
	griddata->setRowStretch(1, 1);
	griddata->setHorizontalSpacing(50);
	griddata->setColumnStretch(1, 1);

	mainhbox->addLayout(std::move(griddata));

	//root()->addWidget(Wt::cpp14::make_unique<Wt::WBreak>());

	auto undoButton = root()->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "Undo Move"));
	auto undoIcon_ = undoButton->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "undo"));
	undoIcon_->setHtmlTagName("i");
	undoIcon_->setStyleClass("material-icons left");
	undoButton->setStyleClass("waves-effect brown waves-light btn-small z-depth-2");
	undoButton->setMargin(10, Wt::Side::Right);
	
	auto setFENButton = root()->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "Set Custom Position"));
	auto setFENIcon_ = setFENButton->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "perm_data_setting"));
	setFENIcon_->setHtmlTagName("i");
	setFENIcon_->setStyleClass("material-icons left");
	setFENButton->setStyleClass("waves-effect brown waves-light btn-small z-depth-2");
	setFENButton->setMargin(10, Wt::Side::Right);

	setFENButton->clicked().connect(chessboard_, [=]() {
		showSetFENPopup();
	});

	//root()->addWidget(Wt::cpp14::make_unique<Wt::WBreak>());    // insert a line break
	//root()->addWidget(Wt::cpp14::make_unique<Wt::WBreak>());

	undoButton->clicked().connect(chessboard_, [=]()
	{
		chessboard_->undoMove(false);
	});

	chessboard_->disableUndoSignal_.connect([=](std::string status) {
		if (status == "disable")
		{
			undoButton->disable();
			newGameButton->disable();
			setFENButton->disable();
		}
		else
		{
			undoButton->enable();
			newGameButton->enable();
			setFENButton->enable();
		}
	});
	/*
	 * Connect signals with slots
	 *
	 * - simple Wt-way: specify object and method
	 */
	newGameButton->clicked().connect(this, &ChessApplication::NewGameClicked);


}