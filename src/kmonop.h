#ifndef __KMONOP_KMONOP_H__
#define __KMONOP_KMONOP_H__

#include <qwidget.h>
#include <qlayout.h>
#include <qtextview.h>
#include <qdom.h>

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
		
	public slots:
		void slotNewGame();
		void slotConfigure();
		void slotRoll();
		void slotBuy();
		void slotSendMsg();
		void slotMsgError(QString);
		void slotMsgInfo(QString);
		void slotMsgStartGame(QString);
		void slotMsgPlayerUpdate(QDomNode);
		void slotMsgEstateUpdate(QDomNode);
		void slotSetPlayerId(int);
		void slotSetTurn(int);
	
	private:
		QWidget *main;
		QLineEdit *input;
		QGridLayout *layout;
		QTextView *output;
		
		KAction *roll_die, *buy_estate, *config_kmonop;

		NewGameWizard *wizard;
		ConfigDialog *configDialog;

		GameNetwork *netw;
		PortfolioView *port[6];
		KMonopBoard *board;
		int myPlayerId;
};

#endif
