#include <qvgroupbox.h>

#include <klocale.h>
#include <kdebug.h>

#include "trade_widget.moc"

TradeDisplay::TradeDisplay(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, 10);
	CHECK_PTR(m_mainLayout);

	QVGroupBox *groupBox;
	groupBox = new QVGroupBox(this, "groupBox");
	m_mainLayout->addWidget(groupBox);
	
	m_playerList = new QListView(groupBox, "playerlist");
	m_playerList->addColumn(QString(i18n("Id")));
	m_playerList->addColumn(QString(i18n("Name")));
}

void TradeDisplay::tradeChanged()
{
	kdDebug() << "TradeDisplay::tradeChanged" << endl;
	QListViewItem *item = new QListViewItem(m_playerList, "new", "player");
}
