#ifndef __KMONOP_KMONOP_H__
#define __KMONOP_KMONOP_H__

#include <ktmainwindow.h>
#include <qtextview.h>

#include "newgamedlg.h"
#include "network.h"

class QLabel;

class KMonop : public KTMainWindow
{
	Q_OBJECT
	
	public:
		KMonop(const char *name=0);
		
	public slots:
		void slotNewGame();
		void slotMsgError(QString);
		void slotMsgStartGame(QString);
	
	private:
		QLineEdit *input;
		QTextView *output;
		NewGameWizard *wizard;
		GameNetwork *netw;
};

#endif
