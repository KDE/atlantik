#ifndef ATLANTIK_BOARD_H
#define ATLANTIK_BOARD_H

#include <qwidget.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qptrlist.h>

#include "config.h"
#include "token.h"

class AtlanticCore;
class Auction;
class Estate;
class Player;

class EstateView;

class AtlantikBoard : public QWidget
{
Q_OBJECT

public:
	enum DisplayMode { Play, Edit };

	AtlantikBoard(AtlanticCore *atlanticCore, int maxEstates, DisplayMode mode, QWidget *parent, const char *name=0);
	void setViewProperties(bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects, bool animateTokens);
	int heightForWidth(int);
	void addEstateView(Estate *estate, bool indicateUnowned = false, bool highliteUnowned = false, bool darkenMortgaged = false, bool quartzEffects = false);
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
	void displayCard(QString, QString);
	void displayEstateDetails(Estate *);

signals:
	void tokenConfirmation(Estate *estate);

protected:
	void resizeEvent(QResizeEvent *);
	QWidget *m_center;

private:
	AtlanticCore *m_atlanticCore;
	DisplayMode m_mode;

	EstateView *getEstateView(Estate *estate);
	void jumpToken(Token *, EstateView *estateView, bool confirm=true);
	void moveToken(Token *, int destination);

	QWidget *spacer;
	QGridLayout *m_gridLayout;
	Token *move_token;
	QTimer *m_timer;
	bool m_resumeTimer;

	bool m_animateTokens;
	int m_maxEstates;

	QPtrList<EstateView> m_estateViews;
	QMap<Player *, Token *> tokenMap;
};

#endif
