#ifndef ATLANTIK_AUCTION_WIDGET_H
#define ATLANTIK_AUCTION_WIDGET_H

#include <qwidget.h>
#include <qlayout.h>
#include <qmap.h>

#include <klistview.h>

class QVGroupBox;
class QSpinBox;
class QLabel;

class KListViewItem;

class AtlanticCore;
class Player;
class Auction;

class AuctionWidget : public QWidget
{
Q_OBJECT

public:
	AuctionWidget(AtlanticCore *atlanticCore, Auction *auction, QWidget *parent, const char *name=0);

private slots:
	void auctionChanged();
	void playerChanged(Player *player);
	void updateBid(Player *player, int amount);
	void slotBidButtonClicked();

signals:
	void bid(Auction *auction, int amount);

private:
	QVBoxLayout *m_mainLayout;
	QVGroupBox *m_playerGroupBox;
	QSpinBox *m_bidSpinBox;
	QMap<Player *, KListViewItem *> m_playerItems;
	QLabel *m_statusLabel;

	KListView *m_playerList;

	AtlanticCore *m_atlanticCore;
	Auction *m_auction;
};

#endif
