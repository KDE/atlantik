#include <qtooltip.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwmatrix.h>
#include <qpopupmenu.h>

#include <kpixmapeffect.h>
#include <kstddirs.h>
#include <kpopupmenu.h>
#include <klocale.h>

#include "estateview.h"
#include "network.h"
#include "config.h"

extern QColor kmonop_greenbg, kmonop_redhotel, kmonop_greenhouse;
extern QColor kmonop_lgray;
extern KMonopConfig kmonopConfig;

EstateView::EstateView(int id, int orientation, bool canBeOwned, const QColor &color, const QString &_icon, QWidget *parent, const char *name) : QWidget(parent, name, WResizeNoErase)
{
	m_id = id;
	m_orientation = orientation;
	m_canBeOwned = canBeOwned;
	m_mortgaged = false;
	m_canBeMortgaged = false;
	m_canBeUnmortgaged = false;
	m_color = color;

	setBackgroundMode(NoBackground); // avoid flickering

	b_recreate = true;
	qpixmap = 0;

	lname = new QLabel(this);
	lname->setAlignment(Qt::AlignLeft);
	lname->setMinimumSize(lname->sizeHint());
	lname->setMaximumWidth(width());
	lname->setMaximumHeight(15);
	lname->hide();

	pe = 0;
	icon = new QPixmap(locate("data", "kmonop/pics/" + _icon));
	icon = rotatePixmap(icon);

	// Initialize Quartz blocks, regardless of configuration (when user
	// enabled effects pixmaps must've been initialized)
	m_quartzBlocks = new KPixmap();
	m_quartzBlocks->resize(25, 18);
	if (color.isValid())
	{
		drawQuartzBlocks(m_quartzBlocks, *m_quartzBlocks, color.light(60), color);
		m_quartzBlocks = rotatePixmap(m_quartzBlocks);
	}

	setName("");
	setHouses(0);
	setOwned(false, false);

	QToolTip::add(this, estatename);
}

QPixmap *EstateView::rotatePixmap(QPixmap *p)
{
	if (p->isNull())
		return 0;

	QWMatrix m;

	switch(m_orientation)
	{
		case East:
			m.rotate(90);
			break;
		case West:
			m.rotate(-90);
			break;
		case South:
			m.rotate(180);
			break;
	}
	*p = p->xForm(m);
	return p;
}

KPixmap *EstateView::rotatePixmap(KPixmap *p)
{
	if (p->isNull())
		return 0;

	QWMatrix m;

	switch(m_orientation)
	{
		case East:
			m.rotate(90);
			break;
		case West:
			m.rotate(-90);
			break;
		case South:
			m.rotate(180);
			break;
	}
	*p = p->xForm(m);
	return p;
}

void EstateView::setName(const char *n)
{
	estatename.setLatin1(n, strlen(n));
	QToolTip::remove(this);
	QToolTip::add(this, estatename);
	lname->setText(n);
}

void EstateView::setHouses(int _h)
{
	if (m_houses != _h)
	{
		m_houses = _h;
		b_recreate = true;
		update();
	}
}

void EstateView::setMortgaged(bool _m)
{
	if (m_mortgaged != _m)
	{
		m_mortgaged = _m;
		b_recreate = true;
		update();
	}
}

void EstateView::setCanBeMortgaged(bool _m)
{
	if (m_canBeMortgaged != _m)
		m_canBeMortgaged = _m;
}

void EstateView::setCanBeUnmortgaged(bool _m)
{
	if (m_canBeUnmortgaged != _m)
		m_canBeUnmortgaged = _m;
}

void EstateView::setOwned(bool byAny, bool byThisClient)
{
	if (m_ownedByThisClient != byThisClient)
		m_ownedByThisClient = byThisClient;

	if (m_ownedByAny != byAny)
	{
		m_ownedByAny = byAny;
		b_recreate = true;
		update();
	}
	updatePE();
}

bool EstateView::mortgaged() { return m_mortgaged; }
bool EstateView::canBeMortgaged() { return m_canBeMortgaged; }
bool EstateView::canBeUnmortgaged() { return m_canBeUnmortgaged; }
bool EstateView::ownedByAny() { return m_ownedByAny; }
bool EstateView::ownedByThisClient() { return m_ownedByThisClient; }

void EstateView::updatePE()
{
	// Don't show a when a property is not unowned, cannot be owned at all
	// or when the user has configured KMonop not to show them.
	if (m_ownedByAny || !m_canBeOwned || kmonopConfig.indicateUnowned==false)
	{
		delete pe;
		pe = 0;
	}
	else
	{
		if (pe==0)
		{
			// Display a coloured portfolioestate to indicate property is
			// for sale
			pe = new PortfolioEstate(this);
			pe->setColor(m_color);
			pe->setOwned(true);
			repositionPortfolioEstate();
			pe->show();
		}
		else if (!pe->isVisible())
			pe->show();
	}
}

void EstateView::redraw()
{
	b_recreate = true;
    update();
}

void EstateView::repositionPortfolioEstate()
{
	if (pe!=0)
	{
		int x = (m_orientation == West ? (width()-2 - pe->width()) : 2);
		int y = (m_orientation == North ? (height()-2 - pe->height()) : 2);
		pe->setGeometry(x, y, pe->width(), pe->height());
	}
}

void EstateView::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(Qt::black);
		
		if (kmonopConfig.grayOutMortgaged==true && m_mortgaged)
			painter.setBrush(kmonop_lgray);
		else if (kmonopConfig.highliteUnowned==true && m_canBeOwned && !m_ownedByAny)
			painter.setBrush(Qt::white);
		else
			painter.setBrush(kmonop_greenbg);

		painter.drawRect(rect());
        
		// Paint icon only when it exists and fits
		if (icon!=0 && width() > icon->width() && height() > icon->height())
			painter.drawPixmap( (width() - icon->width())/2, (height() - icon->height())/2, *icon);

		if (m_color.isValid())
		{
			int titleHeight = height()/4;
			int titleWidth = width()/4;

			KPixmap* quartzBuffer = new KPixmap;
			if (m_orientation == North || m_orientation == South)
				quartzBuffer->resize(25, titleHeight-2);
			else
				quartzBuffer->resize(titleWidth-2, 25);
			QPainter quartzPainter(quartzBuffer, this);

			painter.setBrush(m_color);
			switch(m_orientation)
			{
				case North:
					painter.drawRect(0, 0, width(), titleHeight);

					if (kmonopConfig.quartzEffects)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(1, 1, *quartzBuffer);
					}

					if (m_houses > 0)
					{
						if (m_houses == 5)
						{
							// Hotel
							painter.setBrush(kmonop_redhotel);
							painter.drawRect(2, 2, (width()/2)-4, (titleHeight)-4);
						}
						else
						{
							// Houses
							painter.setBrush(kmonop_greenhouse);
							int h = (titleHeight)-4, w = (titleWidth)-4;
							for( int i=0 ; i< m_houses ; i++ )
								painter.drawRect(2+(i*(w+2)), 2, w, h);
						}
					}
					break;
				case South:
					painter.drawRect(0, height()-(titleHeight), width(), titleHeight);

					if (kmonopConfig.quartzEffects)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(width()-quartzBuffer->width()-1, height()-titleHeight+1, *quartzBuffer);
					}

					if (m_houses > 0)
					{
						if (m_houses == 5)
						{
							// Hotel
							painter.setBrush(kmonop_redhotel);
							painter.drawRect(2, (3*(titleHeight))+2, (width()/2)-4, (titleHeight)-4);
						}
						else
						{
							// Houses
							painter.setBrush(kmonop_greenhouse);
							int h = (titleHeight)-4, w = (titleWidth)-4;
							for( int i=0 ; i< m_houses ; i++ )
								painter.drawRect(2+(i*(w+2)), (3*(titleHeight))+2, w, h);
						}
					}
					break;
				case West:
					painter.drawRect(0, 0, titleWidth, height());

					if (kmonopConfig.quartzEffects)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(1, height()-quartzBuffer->height()-1, *quartzBuffer);
					}

					if (m_houses > 0)
					{
						if (m_houses == 5)
						{
							// Hotel
							painter.setBrush(kmonop_redhotel);
							painter.drawRect(2, 2, (titleWidth)-4, (height()/2)-4);
						}
						else
						{
							// Houses
							painter.setBrush(kmonop_greenhouse);
							int h = (titleHeight)-4, w = (titleWidth)-4;
							for( int i=0 ; i< m_houses ; i++ )
								painter.drawRect(2, 2+(i*(h+2)), w, h);
						}
					}
					break;
				case East:
					painter.drawRect(width()-(titleWidth), 0, titleWidth, height());

					if (kmonopConfig.quartzEffects)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(width()-quartzBuffer->width()-1, 1, *quartzBuffer);
					}

					if (m_houses > 0)
					{
						if (m_houses == 5)
						{
							// Hotel
							painter.setBrush(kmonop_redhotel);
							painter.drawRect((3*(titleWidth))+2, 2, (titleWidth)-4, (height()/2)-4);
						}
						else
						{
							// Houses
							painter.setBrush(kmonop_greenhouse);
							int h = (titleHeight)-4, w = (titleWidth)-4;
							for( int i=0 ; i< m_houses ; i++ )
								painter.drawRect((3*(titleWidth))+2, 2+(i*(h+2)), w, h);
						}
					}
					break;
			}
			quartzPainter.end();
			delete quartzBuffer;
		}
		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void EstateView::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
	
	QTimer::singleShot(0, this, SLOT(slotResizeAftermath()));
}

void EstateView::mousePressEvent(QMouseEvent *e) 
{
	if (e->button()==RightButton)
	{
		KPopupMenu *rmbMenu = new KPopupMenu(this);
		rmbMenu->insertTitle(estatename);
		if (m_mortgaged)
			rmbMenu->insertItem(i18n("Unmortgage"), 0);
		else
			rmbMenu->insertItem(i18n("Mortgage"), 0);

		if (m_houses>=4)
			rmbMenu->insertItem(i18n("Build hotel"), 1);
		else
			rmbMenu->insertItem(i18n("Build house"), 1);

		if (m_houses==5)
			rmbMenu->insertItem(i18n("Sell hotel"), 2);
		else
			rmbMenu->insertItem(i18n("Sell house"), 2);

		// Disable all if we don't own it
		if (!m_ownedByThisClient)
		{
			rmbMenu->setItemEnabled(0, false);
			rmbMenu->setItemEnabled(1, false);
			rmbMenu->setItemEnabled(2, false);
		}
		else
		{
			// Mortgaged or full? Not building any houses
			if (!(m_canBeMortgaged || m_canBeUnmortgaged))
				rmbMenu->setItemEnabled(0, false);

			// TODO: can_build/can_sell monopd support
			// Empty? Not selling any houses
			if (m_houses==0)
				rmbMenu->setItemEnabled(2, false);
		}

		connect(rmbMenu, SIGNAL(activated(int)), this, SLOT(slotMenuAction(int)));
		QPoint g = QCursor::pos();
		rmbMenu->exec(g);
	}
}

void EstateView::slotResizeAftermath()
{
	m_titleWidth = width()/4;
	m_titleHeight = height()/4;
	
	repositionPortfolioEstate();
}

void EstateView::slotMenuAction(int item)
{
	switch (item)
	{
	case 0:
		if (m_mortgaged)
			gameNetwork->cmdEstateUnmortgage(m_id);
		else
			gameNetwork->cmdEstateMortgage(m_id);
		break;

	case 1:
		gameNetwork->cmdHouseBuy(m_id);
		break;

	case 2:
		gameNetwork->cmdHouseSell(m_id);
		break;
	}
}

// Kudos to Gallium <gallium@kde.org> for writing the Quartz KWin style and
// letting me use the ultra slick algorithm!
void EstateView::drawQuartzBlocks(KPixmap *pi, KPixmap &p, const QColor &c1, const QColor &c2)
{
	QPainter px;

	px.begin(pi);

	KPixmapEffect::gradient(p, c1, c2, KPixmapEffect::HorizontalGradient);

	px.fillRect( 2, 1, 3, 3, c1.light(120) );
	px.fillRect( 2, 5, 3, 3, c1 );
	px.fillRect( 2, 9, 3, 3, c1.light(110) );
	px.fillRect( 2, 13, 3, 3, c1 );

	px.fillRect( 6, 1, 3, 3, c1.light(110) );
	px.fillRect( 6, 5, 3, 3, c2.light(110) );
	px.fillRect( 6, 9, 3, 3, c1.light(120) );
	px.fillRect( 6, 13, 3, 3, c2.light(130) );

	px.fillRect( 10, 5, 3, 3, c1.light(110) );
	px.fillRect( 10, 9, 3, 3, c2.light(120) );
	px.fillRect( 10, 13, 3, 3, c2.light(150) );

	px.fillRect( 14, 1, 3, 3, c1.dark(110) );
	px.fillRect( 14, 9, 3, 3, c2.light(120) );
	px.fillRect( 14, 13, 3, 3, c1.dark(120) );

	px.fillRect( 18, 5, 3, 3, c1.light(110) );
	px.fillRect( 18, 13, 3, 3, c1.dark(110) );

	px.fillRect( 22, 9, 3, 3, c2.light(120));
	px.fillRect( 22, 13, 3, 3, c2.light(110) );
}
