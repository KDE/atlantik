#ifndef ATLANTIK_BOARD_H
#define ATLANTIK_BOARD_H

#include <qwidget.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qptrlist.h>

#include "config.h"
//#include "estateview.h"
#include "token.h"

class Estate;
class EstateView;
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
	QWidget *centerWidget() { return m_center; };
	QPtrList<EstateView> estateViews();

	public slots:
		void slotMoveToken();
		void slotResizeAftermath();

	private slots:
		void playerChanged();
		void displayCenter();
		void slotDisplayCard(QString, QString);

	signals:
		void tokenConfirmation(Estate *estate);

	protected:
		void resizeEvent(QResizeEvent *);
		QWidget *m_center;

private:
	EstateView *getEstateView(Estate *estate);
	void jumpToken(Token *, EstateView *estateView, bool confirm=true);
	void moveToken(Token *, int destination);

	QWidget *spacer;
	QGridLayout *m_gridLayout;
	Token *move_token;
	QTimer *m_timer;
	bool m_resumeTimer;

	QPtrList<EstateView> m_estateViews;
	QMap<int, Token *> tokenMap;
};

#endif
