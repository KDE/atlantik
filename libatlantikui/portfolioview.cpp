#include <qpainter.h>
#include <qcursor.h>

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include <config.h>
#include <player.h>
#include <estate.h>

#include "portfolioview.moc"

#define PE_DISTW	4
#define	PE_DISTH	4
#define PE_SPACE	2
#define PE_MARGINW	5
#define PE_MARGINH	2

PortfolioView::PortfolioView(Player *player, QColor activeColor, QColor inactiveColor, QWidget *parent, const char *name) : QWidget(parent, name)
{
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
}

Player *PortfolioView::player()
{
	return m_player;
}

// TODO: little idea for later when we actually know about the groups beforehand
/*
void PortfolioView::buildPortfolio()
{
	// Loop through estategroups in order
	// For each entry loop through estates and add estates
}
*/

void PortfolioView::addEstateView(Estate *estate)
{
	int estateId = estate->estateId();
	if (!estate->canBeOwned() || estate->group().isEmpty())
		return;

	kdDebug() << estate->name() << " canBeOwned and group is not empty but \"" << estate->group() << "\"" << endl;

	// Create PE
	PortfolioEstate *portfolioEstate = new PortfolioEstate(estate, m_player, false, this, "portfolioestate");
	portfolioEstateMap[estateId] = portfolioEstate;

	// Find the last PE with the same group
	PortfolioEstate *tmpPE = 0, *referencePE = 0;
	for (QMap<int, PortfolioEstate *>::Iterator i=portfolioEstateMap.begin() ; i != portfolioEstateMap.end() ; ++i)
		{
			if ((tmpPE = *i))
			{
				// Estate found, break
				if (tmpPE->estate() == estate)
					break;

				if (tmpPE->estate()->group() == estate->group())
				{
					// Same group, so set referencePE and if current
					// referencePE is the very last PE, we're now it.
					if (referencePE == m_lastPE)
						m_lastPE = tmpPE;
					referencePE = tmpPE;
				}
			}
		}

	// If we found a reference PE, we can place the new one next to it
	if (referencePE)
	{
		kdDebug() << "last of same group (" << estate->group() << ")" << endl;
		x = referencePE->x()+2;
		y = referencePE->y()+4;
	}
	// No reference PE from the same group, so place new one aside the last one
	else
	{
		if (m_lastPE)
		{
			// Next to m_lastPE
			kdDebug() << "last of any" << endl;
			x = m_lastPE->x() + 18;
			y = 18;
		}
		else
		{
			// No m_lastPE, place at the defaults
			kdDebug() << "default" << endl;
			x = 5;
			y = 18;
		}

		// We're the last one now
		m_lastPE = portfolioEstate;
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
	QString name;
	name.setNum(m_player->playerId());
	name.append(". " + m_player->name());

	m_nameLabel->setText(name);
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
