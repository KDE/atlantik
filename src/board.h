#ifndef ATLANTIK_BOARD_H
#define ATLANTIK_BOARD_H

#include <qwidget.h>
#include <qtimer.h>
#include <qabstractlayout.h>

#include "config.h"
#include "estateview.h"
#include "token.h"

class AtlantikBoard : public QWidget
{
Q_OBJECT

	public:
		AtlantikBoard(QWidget *parent, const char *name=0);
		void setOwned(int estateId, bool byAny, bool byThisClient);
		void raiseToken(int);
		void indicateUnownedChanged();
		void redrawEstates();

	public slots:
		void slotMsgPlayerUpdateLocation(int, int, bool);
		void slotMsgEstateUpdateName(int, QString);
		void slotMsgEstateUpdateHouses(int, int);
		void slotMsgEstateUpdateMortgaged(int, bool);
		void slotMsgEstateUpdateCanBeMortgaged(int, bool);
		void slotMsgEstateUpdateCanBeUnmortgaged(int, bool);
		void slotMoveToken();
		void slotResizeAftermath();

	protected:
		void resizeEvent(QResizeEvent *);

	private:
		void jumpToken(Token *, int destination, bool confirm=true);
		void moveToken(Token *, int destination);

		QWidget *spacer, *center;
		EstateView *estate[40];
		Token *token[MAXPLAYERS], *move_token;
		QTimer *m_timer;
		bool m_resumeTimer;
};

#endif
