#include "SetFENPopup.h"
#include <Wt/WAnchor.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
SetFENPopup::~SetFENPopup()
{
	// delete dialog_;
}
SetFENPopup::SetFENPopup(std::string strFEN,Wt::WApplication *ptrApp) : WObject()
{
	selectedFEN = (strFEN == "") ? "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" : strFEN;
	whitekingside = true;
	whitequeenside = true;
	blackkingside = true;
	blackqueenside = true;

	dialog_ = addChild(Wt::cpp14::make_unique<Wt::WDialog>("Set Position"));
	dialog_->contents()->setWidth(800);
        dialog_->escapePressed().connect(dialog_, &Wt::WDialog::reject);
        
        auto fenlabel = dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WText>("<h5>FEN : </h5>"));
        fenlabel->setAttributeValue("style", "display : inline-block");
        auto LoadFENcontainer = dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WContainerWidget>());

        fenEdit = LoadFENcontainer->addWidget(Wt::cpp14::make_unique<Wt::WLineEdit>());
	fenEdit->setText(selectedFEN);
        fenEdit->setWidth(500);
        
        LoadFENcontainer->setStyleClass("input-field inline");
        auto loadFENButton = dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "Load FEN"));
	loadFENButton->setStyleClass("waves-effect brown waves-light btn-small z-depth-2");
	loadFENButton->setMargin(10, Wt::Side::Left);
	loadFENButton->clicked().connect([&]() {
            
            std::vector<std::string> result; 
            std::istringstream iss(fenEdit->text().toUTF8()); 
            for(std::string s; iss >> s; ) 
                result.push_back(s); 
            if (result.size() != 6)
            {
                Wt::StandardButton answer = Wt::WMessageBox::show("Chess-Error","Invalid FEN.", Wt::StandardButton::Ok);
                return;
            }	
            std::string toMove = result[1];
            group1->setCheckedButton(group1->button(static_cast<int>((toMove == "w") ? SetFENPopup::Turn::WHITE : SetFENPopup::Turn::BLACK)));        
            
            std::string castlinginfo = result[2];
            c1->setChecked(castlinginfo.find("K") != std::string::npos);
            whitekingside = c1->isChecked();
            c2->setChecked(castlinginfo.find("Q") != std::string::npos);
            whitequeenside = c2->isChecked();
            c3->setChecked(castlinginfo.find("k") != std::string::npos);
            blackkingside = c3->isChecked();
            c4->setChecked(castlinginfo.find("q") != std::string::npos);
            blackqueenside = c4->isChecked();
	    
            chessboard_->loadFEN(fenEdit->text().toUTF8());
                
	});
	auto trainingcontainer = dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WContainerWidget>());

	auto mainhbox = trainingcontainer->setLayout(Wt::cpp14::make_unique<Wt::WHBoxLayout>());

	chessboard_ = mainhbox->addWidget(Wt::cpp14::make_unique<TrainingWidget>(ptrApp));
	if (strFEN != "")
		chessboard_->loadFEN(strFEN);
	chessboard_->setWidth(Wt::WLength(30, Wt::LengthUnit::Percentage));

	chessboard_->updateFEN->connect([&](std::string fen) {
		selectedFEN = fen;
	});

	auto vbox = Wt::cpp14::make_unique <Wt::WVBoxLayout>();
	vbox->addWidget(Wt::cpp14::make_unique<Wt::WText>("<h4>Turn : </h4>"));
	group1 = std::make_shared<Wt::WButtonGroup>();

	option1 = vbox->addWidget(Wt::cpp14::make_unique<Wt::WRadioButton>("White to Move"));
	group1->addButton(option1, static_cast<int>(SetFENPopup::Turn::WHITE));
	option2 = vbox->addWidget(Wt::cpp14::make_unique<Wt::WRadioButton>("Black to Move"));
	group1->addButton(option2, static_cast<int>(SetFENPopup::Turn::BLACK));

	vbox->addWidget(Wt::cpp14::make_unique<Wt::WText>("<h4>Castling Availability : </h4>"));

	c1 = vbox->addWidget(Wt::cpp14::make_unique<Wt::WCheckBox>("White - King side"));
	c1->changed().connect([&]()
	{
		whitekingside = (c1->isChecked()) ? true : false;
	});

	c2 = vbox->addWidget(Wt::cpp14::make_unique<Wt::WCheckBox>("White - Queen side"));
	c2->changed().connect([&]()
	{
		whitequeenside = (c2->isChecked()) ? true : false;
	});
	c3 = vbox->addWidget(Wt::cpp14::make_unique<Wt::WCheckBox>("Black - King side"));
	c3->changed().connect([&]()
	{
		blackkingside = (c3->isChecked()) ? true : false;
	}); 
	c4 = vbox->addWidget(Wt::cpp14::make_unique<Wt::WCheckBox>("Black - Queen side"));
	c4->changed().connect([&]()
	{
		blackqueenside = (c4->isChecked()) ? true : false;
	});

        std::vector<std::string> result; 
        std::istringstream iss(fenEdit->text().toUTF8()); 
        for(std::string s; iss >> s; ) 
            result.push_back(s); 
        if (result.size() == 6)
        {
            	
            std::string toMove = result[1];
            group1->setCheckedButton(group1->button(static_cast<int>((toMove == "w") ? SetFENPopup::Turn::WHITE : SetFENPopup::Turn::BLACK)));        

            std::string castlinginfo = result[2];
            c1->setChecked(castlinginfo.find("K") != std::string::npos);
            whitekingside = c1->isChecked();
            c2->setChecked(castlinginfo.find("Q") != std::string::npos);
            whitequeenside = c2->isChecked();
            c3->setChecked(castlinginfo.find("k") != std::string::npos);
            blackkingside = c3->isChecked();
            c4->setChecked(castlinginfo.find("q") != std::string::npos);
            blackqueenside = c4->isChecked();
            
        }



	mainhbox->addLayout(std::move(vbox));
	   

	auto okButton = dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "Start Game with Position"));
	dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WText>(""));
	auto cancelButton = dialog_->contents()->addWidget(Wt::cpp14::make_unique<Wt::WAnchor>("", "Cancel"));
	
	okButton->setStyleClass("waves-effect brown waves-light btn-small z-depth-2");
	okButton->setMargin(10, Wt::Side::Right);
	okButton->clicked().connect(dialog_, &Wt::WDialog::accept);
	
	cancelButton->setStyleClass("waves-effect brown waves-light btn-small z-depth-2");
	cancelButton->clicked().connect(dialog_, &Wt::WDialog::reject);
		
	dialog_->finished().connect(this, &SetFENPopup::dialogDone);
	dialog_->setOffsets(300, Wt::Side::Left);
	dialog_->setOffsets(30, Wt::Side::Top);
	dialog_->show();
}
void SetFENPopup::dialogDone(Wt::DialogCode code)
{
	if (code == Wt::DialogCode::Accepted)
	{
		bool noCastle = !whitekingside && !whitequeenside && !blackkingside && !blackqueenside;
		selectedFEN = selectedFEN + " " + ((group1->checkedId() == static_cast<int>(SetFENPopup::Turn::WHITE)) ? "w" : "b");
		if (!noCastle)
		{
			selectedFEN = selectedFEN + " " + (whitekingside ? "K" : "");
			selectedFEN = selectedFEN + (whitequeenside ? "Q" : "");
			selectedFEN = selectedFEN + (blackkingside ? "k" : "");
			selectedFEN = selectedFEN + (blackqueenside ? "q" : "");
		}
		else
			selectedFEN = selectedFEN + " -";
		selectedFEN = selectedFEN + " - 0 1";  // Yet to support recording 2 square pawn move, and move count
		fenselected_.emit(selectedFEN);
	}
	removeChild(dialog_);
}
