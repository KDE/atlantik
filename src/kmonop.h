#ifndef __KMONOP_KMONOP_H__
#define __KMONOP_KMONOP_H__

#include <qwidget.h>
#include <qlayout.h>
#include <qtextview.h>

#include <ktmainwindow.h>
#include <kaction.h>

#include "configdlg.h"
#include "newgamedlg.h"
#include "network.h"
#include "portfolioview.h"
#include "board.h"

class QLabel;

class KMonop : public KTMainWindow
{
	Q_OBJECT
	
	public:
		KMonop(const char *name=0);
		void readConfig();
		
	public slots:
		void slotNewGame();

		void slotConfigure();
		void slotUpdateConfig();

		void slotRoll();
		void slotBuy();
		void slotEndTurn();
		void slotSendMsg();

		void slotMsgError(QString);
		void slotMsgInfo(QString);
		void slotMsgChat(QString, QString);
		void slotMsgStartGame(QString);
		void slotMsgPlayerUpdateName(int, QString);
		void slotMsgPlayerUpdateMoney(int, QString);
		void slotMsgEstateUpdateOwner(int, int);

		void slotSetPlayerId(int);
		void slotSetTurn(int);
	
	private:
		void outputAppend(QString);

		QWidget *main;
		QLineEdit *input;
		QGridLayout *layout;
		QTextView *output;
		
		KAction *roll_die, *buy_estate, *end_turn, *config_kmonop;

		NewGameWizard *wizard;
		ConfigDialog *configDialog;

		PortfolioView *port[6];
		KMonopBoard *board;
		int myPlayerId;
};

#endif
