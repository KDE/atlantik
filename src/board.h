#ifndef ATLANTIK_BOARD_H
#define ATLANTIK_BOARD_H

#include <qwidget.h>
#include <qtimer.h>
#include <qlayout.h>

#include "config.h"
#include "estateview.h"
#include "token.h"

class Estate;
class Player;

class AtlantikBoard : public QWidget
{
Q_OBJECT

	public:
		AtlantikBoard(QWidget *parent, const char *name=0);
		void addEstateView(Estate *estate);
		void addToken(Player *player);
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
		Token *move_token;
		QTimer *m_timer;
		bool m_resumeTimer;

		QMap<int, EstateView *> estateViewMap;
		QMap<int, Token *> tokenMap;
};

#endif
