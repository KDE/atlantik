#ifndef ATLANTIK_BOARD_H
#define ATLANTIK_BOARD_H

#include <qwidget.h>
#include <qtimer.h>
#include <qlayout.h>

#include "config.h"
#include "estateview.h"
#include "token.h"

class Estate;

class AtlantikBoard : public QWidget
{
Q_OBJECT

	public:
		AtlantikBoard(QWidget *parent, const char *name=0);
		void addEstateView(Estate *estate);
		void setOwned(int estateId, bool byAny, bool byThisClient);
		void raiseToken(int);
		void indicateUnownedChanged();

	public slots:
		void slotMsgPlayerUpdateLocation(int, int, bool);
		void slotMoveToken();
		void slotResizeAftermath();

	signals:
		void tokenConfirmation(int location);

	protected:
		void resizeEvent(QResizeEvent *);

	private:
		void jumpToken(Token *, int destination, bool confirm=true);
		void moveToken(Token *, int destination);

		QWidget *spacer, *center;
		QGridLayout *m_gridLayout;
		Token *token[MAXPLAYERS], *move_token;
		QTimer *m_timer;
		bool m_resumeTimer;
};

#endif
