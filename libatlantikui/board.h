#ifndef ATLANTIK_BOARD_H
#define ATLANTIK_BOARD_H

#include <qwidget.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qptrlist.h>

#include "config.h"
#include "token.h"

class AtlanticCore;

class EstateView;

class Player;
class Estate;
class Auction;

class AtlantikBoard : public QWidget
{
Q_OBJECT

public:
	AtlantikBoard(AtlanticCore *atlanticCore, int, QWidget *parent, const char *name=0);
	int heightForWidth(int);
	void addEstateView(Estate *estate);
	void addAuctionWidget(Auction *auction);
	void addToken(Player *player);
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
	void displayEstateDetails(Estate *);

signals:
	void tokenConfirmation(Estate *estate);

protected:
	void resizeEvent(QResizeEvent *);
	QWidget *m_center;

private:
	AtlanticCore *m_atlanticCore;

	EstateView *getEstateView(Estate *estate);
	void jumpToken(Token *, EstateView *estateView, bool confirm=true);
	void moveToken(Token *, int destination);

	QWidget *spacer;
	QGridLayout *m_gridLayout;
	Token *move_token;
	QTimer *m_timer;
	bool m_resumeTimer;

	int m_maxEstates;

	QPtrList<EstateView> m_estateViews;
	QMap<Player *, Token *> tokenMap;
};

#endif
