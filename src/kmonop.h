#include <ktmainwindow.h>
#include <qtextview.h>
#include <qsocket.h>

#include "newgamedlg.h"

class QLabel;

class KMonop : public KTMainWindow
{
	Q_OBJECT
	
	public:
		KMonop(const char *name=0);
		
	public slots:
//		void slotRepositionText();
		void slotConnected();
		void slotRead();
		void slotWrite();
		void slotNewGame();
		void slotCloseNewGameDlg();
	
	private:
		QSocket *sock;
		QLabel *serverlabel;
		QLineEdit *server;
		QTextView *output;
};
