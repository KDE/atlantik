#include <qhgroupbox.h>
#include <qspinbox.h>

#include <kdebug.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kpushbutton.h>

#include <atlantic_core.h>
#include <player.h>
#include <auction.h>
 
#include "auction_widget.moc"

AuctionWidget::AuctionWidget(AtlanticCore *atlanticCore, Auction *auction, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_atlanticCore = atlanticCore;

	m_auction = auction;
	connect(m_auction, SIGNAL(updateBid(Player *, int)), this, SLOT(updateBid(Player *, int)));
	connect(this, SIGNAL(bid(Auction *, int)), m_auction, SIGNAL(bid(Auction *, int)));

	m_mainLayout = new QVBoxLayout(this, 10);
	CHECK_PTR(m_mainLayout);

	// Player list.
	m_playerGroupBox = new QVGroupBox(i18n("Auction"), this, "groupBox");
	m_mainLayout->addWidget(m_playerGroupBox); 

	m_playerList = new KListView(m_playerGroupBox);
	m_playerList->addColumn(QString(i18n("Player")));
	m_playerList->addColumn(QString(i18n("Bid")));
	
//	m_label = new QTextEdit("under construction ;)", NULL, m_playerGroupBox);
//	m_label->setReadOnly(true);

	KListViewItem *item;
	Player *player;

	QPtrList<Player> playerList = m_atlanticCore->players();
	for (QPtrListIterator<Player> it(playerList); *it; ++it)
	{
		if ((player = *it))
		{
			item = new KListViewItem(m_playerList, player->name(), QString("0"));
			item->setPixmap(0, QPixmap(SmallIcon("personal")));
			m_playerItems[player] = item;
		}
	}

	QHGroupBox *bidBox = new QHGroupBox(this);
	m_mainLayout->addWidget(bidBox);

	m_bidSpinBox = new QSpinBox(1, 10000, 1, bidBox);

	KPushButton *bidButton = new KPushButton(i18n("Make bid"), bidBox, "bidButton");
	connect(bidButton, SIGNAL(clicked()), this, SLOT(slotBidButtonClicked()));
}

void AuctionWidget::updateBid(Player *player, int amount)
{
	if (!player)
		return;

	QListViewItem *item;
	if (!(item = m_playerItems[player]))
		return;

	item->setText(1, QString::number(amount));
	m_bidSpinBox->setMinValue(amount+1);
	m_playerList->triggerUpdate();
}

void AuctionWidget::slotBidButtonClicked()
{
	emit bid(m_auction, m_bidSpinBox->value());	
}
