#ifndef __KMONOP_KMONOP_H__
#define __KMONOP_KMONOP_H__

#include <ktmainwindow.h>
#include <qtextview.h>
#include <qsocket.h>
#include <qdom.h>

#include "newgamedlg.h"
#include "network.h"

class QLabel;

class KMonop : public KTMainWindow
{
	Q_OBJECT
	
	public:
		KMonop(const char *name=0);
		
	public slots:
		void slotConnected();
		void slotRead();
		void slotWrite();
		void slotNewGame();
		void slotMsgError(QString);
		void slotMsgStartGame(QString);
	
	private:
		QSocket *sock;
		QLabel *serverlabel;
		QLineEdit *server;
		QTextView *output;
		NewGameWizard *wizard;
		GameNetwork *netw;
};

#endif
