#ifndef __KMONOP_BOARD_H__
#define __KMONOP_BOARD_H__

#include <qwidget.h>

#include "config.h"
#include "estateview.h"
#include "network.h"
#include "token.h"

class KMonopBoard : public QWidget
{
Q_OBJECT

	public:
		KMonopBoard(GameNetwork *, QWidget *parent, const char *name=0);

	public slots:
		void slotMsgPlayerUpdate(QDomNode);

	protected:
		void resizeEvent(QResizeEvent *);
		QWidget *spacer;

	private:
		GameNetwork *netw;
		EstateView *estate[40];
		Token *token[MAXPLAYERS];
};

#endif
