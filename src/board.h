#ifndef __KMONOP_BOARD_H__
#define __KMONOP_BOARD_H__

#include <qwidget.h>
#include <qtimer.h>

#include "config.h"
#include "estateview.h"
#include "network.h"
#include "token.h"

class KMonopBoard : public QWidget
{
Q_OBJECT

	public:
		KMonopBoard(GameNetwork *, QWidget *parent, const char *name=0);
		void setOwned(int, bool);
		void raiseToken(int);

	public slots:
		void slotMsgPlayerUpdate(QDomNode);
		void slotMoveToken();
		void slotResizeAftermath();

	protected:
		void resizeEvent(QResizeEvent *);

	private:
		void jumpToken(Token *, int);
		void moveToken(Token *, int);

		QWidget *spacer;
		GameNetwork *netw;
		EstateView *estate[40];
		Token *token[MAXPLAYERS], *move_token;
		QTimer *qtimer;
		bool resume_timer;
};

#endif
