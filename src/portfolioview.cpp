#include <qpainter.h>

#include <kdebug.h>

#include "portfolioview.moc"
#include "player.h"
#include "estate.h"

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

	setFixedSize(QSize(225, 75));
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
}

void PortfolioView::addEstateView(Estate *estate)
{
	kdDebug() << "PortfolioView::addEstateView(" << estate->estateId() << ")" << endl;
	
	int j=0,x=0,y=0, estateId = estate->estateId();
	QColor color = QColor();

	// Don't ask, it works. ;-)
	switch(estateId)
	{
		case 3: case 9:
		case 14: case 19:
		case 24: case 29:
		case 34: case 39:
			x = PE_MARGINW + (2*PE_DISTW) + (((2*PE_DISTW)+PE_SPACE+PE_WIDTH)*(estateId/5));
			y = PE_MARGINH + (0*PE_DISTH) + m_nameLabel->height();
			break;

		case 1: case 8:
		case 13: case 18:
		case 23: case 27:
		case 32: case 37:
			x = PE_MARGINW + (1*PE_DISTW) + (((2*PE_DISTW)+PE_SPACE+PE_WIDTH)*(estateId/5));
			y = PE_MARGINH + (1*PE_DISTH) + m_nameLabel->height();
			break;

		case 6: case 11:
		case 16: case 21:
		case 26: case 31:
			x = PE_MARGINW + (0*PE_DISTW) + (((2*PE_DISTW)+PE_SPACE+PE_WIDTH)*(estateId/5));
			y = PE_MARGINH + (2*PE_DISTH) + m_nameLabel->height();
			break;
			
		case 5: case 15:
		case 25: case 35:
			x = 5+((2+PE_WIDTH)*(estateId/10));
			y = PE_HEIGHT + PE_MARGINH + (3*PE_DISTH) + m_nameLabel->height();
			break;

		case 12:
			x = ((2+PE_WIDTH)*5);
			y = PE_HEIGHT + PE_MARGINH + (3*PE_DISTH) + m_nameLabel->height();
			break;

		case 28:
			x = ((2+PE_WIDTH)*6);
			y = PE_HEIGHT + PE_MARGINH + (3*PE_DISTH) + m_nameLabel->height();
			break;

		default:
			x = 0; y = 0;
	}

	if (x>0 || y>0)
	{
		portfolioEstateMap[estateId] = new PortfolioEstate(this);
		portfolioEstateMap[estateId]->setGeometry(x, y, portfolioEstateMap[estateId]->width(), portfolioEstateMap[estateId]->height());
		portfolioEstateMap[estateId]->setColor(color);
		portfolioEstateMap[estateId]->show();
	}
	else
		portfolioEstateMap[estateId]=0;
}

void PortfolioView::setOwned(int estateId, bool owned)
{
	if (PortfolioEstate *portfolioEstate = portfolioEstateMap[estateId])
		portfolioEstate->setOwned(owned);
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
	kdDebug() << "PortfolioView::playerChanged" << endl;

#warning is this the correct place for label updates?
	QString name;
	name.setNum(m_player->playerId());
	name.append(". " + m_player->name());
	m_nameLabel->setText(name);
	m_moneyLabel->setText(m_player->money());
#warning add label change check
	m_nameLabel->setBackgroundColor(m_player->hasTurn() ? atlantik_dgray : atlantik_lgray);
	m_nameLabel->update();
	m_moneyLabel->setBackgroundColor(m_player->hasTurn() ? atlantik_dgray : atlantik_lgray);
	m_moneyLabel->update();

	b_recreate = true;
	update();
}
