#include <qpainter.h>
#include <qcursor.h>

#include <klocale.h>
#include <kpopupmenu.h>

#include <atlantic_core.h>
#include <config.h>
#include <player.h>
#include <estate.h>
#include <estategroup.h>

#include "portfolioview.moc"

#define PE_DISTW	4
#define	PE_DISTH	4
#define PE_SPACE	2
#define PE_MARGINW	5
#define PE_MARGINH	2

PortfolioView::PortfolioView(AtlanticCore *core, Player *player, QColor activeColor, QColor inactiveColor, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_atlanticCore = core;
	m_player = player;
	m_activeColor = activeColor;
	m_inactiveColor = inactiveColor;
	m_lastPE = 0;

	b_recreate = true;
	qpixmap = 0;

	setFixedSize(QSize(225, 100));
	setBackgroundColor(Qt::white);
	
	m_nameLabel = new QLabel(this);
	m_nameLabel->setAlignment(Qt::AlignLeft);
	m_nameLabel->setGeometry(5, 0, width()/2, height());
	m_nameLabel->setBackgroundColor(m_player->hasTurn() ? m_activeColor : m_inactiveColor);
	m_nameLabel->setMinimumSize(m_nameLabel->sizeHint());
	m_nameLabel->setMaximumWidth(width()-10);
	m_nameLabel->setMaximumHeight(15);
	m_nameLabel->show();

	m_moneyLabel = new QLabel(this);
	m_moneyLabel->setAlignment(Qt::AlignRight);
	m_moneyLabel->setGeometry(width()/2, 0, width()-5, height());
	m_moneyLabel->setBackgroundColor(m_player->hasTurn() ? m_activeColor : m_inactiveColor);
	m_moneyLabel->setMinimumSize(m_moneyLabel->sizeHint());
	m_moneyLabel->setMaximumWidth(width()/2);
	m_moneyLabel->setMaximumHeight(15);
	m_moneyLabel->show();
	
	m_lastGroup = "";
	x = 0;
	y = 0;

	// TODO: call buildPortfolio? Although, we should be able to assume no
	// new estates or players are introduced after the game has been started
}

Player *PortfolioView::player()
{
	return m_player;
}

void PortfolioView::buildPortfolio()
{
	// TODO: clear current portfolioEstateMap
	// Loop through estate groups in order
	QPtrList<EstateGroup> estateGroups = m_atlanticCore->estateGroups();
	PortfolioEstate *lastPE = 0, *firstPEprevGroup = 0;

	EstateGroup *estateGroup;
	for (QPtrListIterator<EstateGroup> it(estateGroups); *it; ++it)
	{
		if ((estateGroup = *it))
		{
			// New group
			lastPE = 0;

			// Loop through estates
			QPtrList<Estate> estates = m_atlanticCore->estates();
			Estate *estate;
			for (QPtrListIterator<Estate> it(estates); *it; ++it)
			{
				if ((estate = *it) && estate->group() == estateGroup->name())
				{
					// Create PE
					PortfolioEstate *portfolioEstate = new PortfolioEstate(estate, m_player, false, this, "portfolioestate");
					portfolioEstateMap[estate->estateId()] = portfolioEstate;

					int x, y;
					if (lastPE)
					{
						x = lastPE->x() + 2;
						y = lastPE->y() + 4;
					}
					else if (firstPEprevGroup)
					{
						x = firstPEprevGroup->x() + PE_WIDTH + 8;
						y = 18;
						firstPEprevGroup = portfolioEstate;
					}
					else
					{
						x = 5;
						y = 18;
						firstPEprevGroup = portfolioEstate;
					}

					portfolioEstate->setGeometry(x, y, portfolioEstate->width(), portfolioEstate->height());
					portfolioEstate->show();

					connect(estate, SIGNAL(changed()), portfolioEstate, SLOT(estateChanged()));

					lastPE = portfolioEstate;
				}
			}
		}
	}
}

/*
void PortfolioView::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}
*/

void PortfolioView::playerChanged()
{
	m_nameLabel->setText(m_player->name());
	m_nameLabel->setBackgroundColor(m_player->hasTurn() ? m_activeColor : m_inactiveColor);
	m_nameLabel->update();

	m_moneyLabel->setText(QString::number(m_player->money()));
	m_moneyLabel->setBackgroundColor(m_player->hasTurn() ? m_activeColor : m_inactiveColor);
	m_moneyLabel->update();

	b_recreate = true;
	update();
}

void PortfolioView::mousePressEvent(QMouseEvent *e) 
{
	if (e->button()==RightButton && !m_player->isSelf())
	{
		KPopupMenu *rmbMenu = new KPopupMenu(this);
		rmbMenu->insertTitle(m_player->name());

		// Start trade
		rmbMenu->insertItem(i18n("Request Trade with %1").arg(m_player->name()), 0);

		connect(rmbMenu, SIGNAL(activated(int)), this, SLOT(slotMenuAction(int)));
		QPoint g = QCursor::pos();
		rmbMenu->exec(g);
	}
}

void PortfolioView::slotMenuAction(int item)
{
	switch (item)
	{
	case 0:
		emit newTrade(m_player);
		break;
	}
}
