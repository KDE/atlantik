#include <kdebug.h>

#include <klocale.h>
 
#include "auction_widget.moc"

AuctionWidget::AuctionWidget(QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, 10);
	CHECK_PTR(m_mainLayout);

	// Player list.
	m_playerGroupBox = new QVGroupBox(i18n("Auction"), this, "groupBox");
	m_mainLayout->addWidget(m_playerGroupBox); 

	m_label = new QTextEdit("under construction ;)", NULL, m_playerGroupBox);
	m_label->setReadOnly(true);
}
