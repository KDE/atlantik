#include <qpainter.h>
#include <qcursor.h>

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include <player.h>
#include <estate.h>

#include "portfolioview.moc"

#define PE_DISTW	4
#define	PE_DISTH	4
#define PE_SPACE	2
#define PE_MARGINW	5
#define PE_MARGINH	2

extern QColor atlantik_lgray, atlantik_dgray;

PortfolioView::PortfolioView(Player *player, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_player = player;

	b_recreate = true;
	qpixmap = 0;

	setFixedSize(QSize(225, 100));
	setBackgroundColor(Qt::white);
	
	m_nameLabel = new QLabel(this);
	m_nameLabel->setAlignment(Qt::AlignLeft);
	m_nameLabel->setGeometry(5, 0, width()/2, height());
	m_nameLabel->setBackgroundColor(atlantik_lgray);
	m_nameLabel->setMinimumSize(m_nameLabel->sizeHint());
	m_nameLabel->setMaximumWidth(width()-10);
	m_nameLabel->setMaximumHeight(15);
	m_nameLabel->show();

	m_moneyLabel = new QLabel(this);
	m_moneyLabel->setAlignment(Qt::AlignRight);
	m_moneyLabel->setGeometry(width()/2, 0, width()-5, height());
	m_moneyLabel->setBackgroundColor(atlantik_lgray);
	m_moneyLabel->setMinimumSize(m_moneyLabel->sizeHint());
	m_moneyLabel->setMaximumWidth(width()/2);
	m_moneyLabel->setMaximumHeight(15);
	m_moneyLabel->show();
	
	m_lastGroupId = 0;
	x = 0;
	y = 0;
}

Player *PortfolioView::player()
{
	return m_player;
}

void PortfolioView::addEstateView(Estate *estate)
{
	int estateId = estate->estateId();
	if (!estate->canBeOwned() || !estate->groupId())
		return;

//	if ((estateId / 10) * 18 > y)
//	{
		y = 18 * (1 + (estateId / 10));
//		x = 0;
//	}

	if (m_lastGroupId >= estate->groupId())
		x += 18;
	else
	{
		x += 36;
		m_lastGroupId = estate->groupId();
	}
	x = 5 + 16 * ((estateId-1) % 10);

	// Create PE
	PortfolioEstate *portfolioEstate =new PortfolioEstate(estate, m_player, false, this, "portfolioestate");
	portfolioEstateMap[estateId] = portfolioEstate;

	// Find last of this group
	PortfolioEstate *tmpPE = 0, *lastPE = 0;
	for (QMap<int, PortfolioEstate *>::Iterator i=portfolioEstateMap.begin() ; i != portfolioEstateMap.end() ; ++i)
		{
			if ((tmpPE = *i))
			{
				if (tmpPE->estate() == estate)
					break;
				if (tmpPE->estate()->groupId() == estate->groupId())
					lastPE = tmpPE;
			}
		}

	// Place this one on top of it
	if (lastPE)
	{
		kdDebug() << "last of same group" << endl;
		x = lastPE->x()+2;
		y = lastPE->y()+4;
	}
	// Else, find last
	else
	{
		for (QMap<int, PortfolioEstate *>::Iterator i=portfolioEstateMap.begin() ; i != portfolioEstateMap.end() ; ++i)
			{
				if ((tmpPE = *i))
				{
					if (tmpPE->estate() == estate)
						break;
					lastPE = tmpPE;
				}
			}
		// Place this one next to it
		if (lastPE)
		{
			kdDebug() << "last of any" << endl;
			x = lastPE->x() + 18;
			y = 18;
		}
		// Else use first placement
		else
		{
			kdDebug() << "default" << endl;
			x = 5;
			y = 18;
		}
	}
	kdDebug() << "setting geometry to " << x << ", " << y << endl;
	portfolioEstate->setGeometry(x, y, portfolioEstate->width(), portfolioEstate->height());

	connect(estate, SIGNAL(changed()), portfolioEstate, SLOT(estateChanged()));
	portfolioEstate->show();
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
#warning is this the correct place for label updates?
	QString name;
	name.setNum(m_player->playerId());
	name.append(". " + m_player->name());
	m_nameLabel->setText(name);
	m_moneyLabel->setText(QString::number(m_player->money()));
#warning add label change check
	m_nameLabel->setBackgroundColor(m_player->hasTurn() ? atlantik_dgray : atlantik_lgray);
	m_nameLabel->update();
	m_moneyLabel->setBackgroundColor(m_player->hasTurn() ? atlantik_dgray : atlantik_lgray);
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
		rmbMenu->insertItem(i18n("Request trade with %1").arg(m_player->name()), 0);

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
