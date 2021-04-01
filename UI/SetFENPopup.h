#include <Wt/WDialog.h>
#include <Wt/WText.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WRadioButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WMessageBox.h>
#include <Wt/WCheckBox.h>
#include "TrainingWidget.h"
class SetFENPopup : public Wt::WObject
{
private:
	bool whitekingside;
	bool whitequeenside;
	bool blackkingside;
	bool blackqueenside;
	Wt::WCheckBox *c1, *c2, *c3, *c4;
	Wt::WRadioButton *option1, *option2;
	Wt::WLineEdit *fenEdit;
	std::shared_ptr<Wt::WButtonGroup> group1;

public:
	enum class Turn { WHITE = 1, BLACK = 2};
	enum class CASTLETYPE {WHITE_KING_SIDE =1 , WHITE_QUEEN_SIDE = 2, BLACK_KING_SIDE = 3, BLACK_QUEEN_SIDE = 4};
	SetFENPopup(std::string strFEN, Wt::WApplication * ptrApp);
	~SetFENPopup();
	TrainingWidget *chessboard_;
	Wt::WLineEdit *fen_;

	Wt::Signal<std::string> fenselected_;
	Wt::WDialog *dialog_;
	std::string selectedFEN;

	void dialogDone(Wt::DialogCode code);

};