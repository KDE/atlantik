#include <qtooltip.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwmatrix.h>
#include <qpopupmenu.h>
#include <qcursor.h>

#include <kdebug.h>

#include <kpixmapeffect.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <klocale.h>

#include "estateview.moc"
#include "estate.h"
#include "network.h"
#include "config.h"

extern QColor atlantik_greenbg, atlantik_redhotel, atlantik_greenhouse;
extern QColor atlantik_lgray;
extern AtlantikConfig atlantikConfig;

EstateView::EstateView(Estate *estate, int orientation, const QString &_icon, QWidget *parent, const char *name) : QWidget(parent, name, WResizeNoErase)
{
	m_estate = estate;
	m_orientation = orientation;

	setBackgroundMode(NoBackground); // avoid flickering

	qpixmap = 0;
	b_recreate = true;

	m_quartzBlocks = 0;	
	m_recreateQuartz = true;

	lname = new QLabel(this);
	lname->setAlignment(Qt::AlignLeft);
	lname->setMinimumSize(lname->sizeHint());
	lname->setMaximumWidth(width());
	lname->setMaximumHeight(15);
	lname->hide();

	pe = 0;
	icon = new QPixmap(locate("data", "atlantik/pics/" + _icon));
	icon = rotatePixmap(icon);

	setOwned(false, false);

	QToolTip::add(this, m_estate->name());

	kdDebug() << "end of estateview ctor" << endl;
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

bool EstateView::ownedByAny() { return m_ownedByAny; }
bool EstateView::ownedByThisClient() { return m_ownedByThisClient; }

void EstateView::updatePE()
{
	// Don't show a when a property is not unowned, cannot be owned at all
	// or when the user has configured Atlantik not to show them.
	if (m_ownedByAny || !m_estate->canBeOwned() || atlantikConfig.indicateUnowned==false)
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
			pe->setColor(m_estate->color());
			pe->setOwned(true);
			repositionPortfolioEstate();
			pe->show();
		}
		else if (!pe->isVisible())
			pe->show();
	}
}

void EstateView::estateChanged()
{
	kdDebug() << "EstateView::estateChanged" << endl;

	// TODO: is this the correct place for name label updates?
	QToolTip::remove(this);
	QToolTip::add(this, m_estate->name());
	lname->setText(m_estate->name());

	b_recreate = true;
	m_recreateQuartz = true;
    update();
}

void EstateView::redraw()
{
	kdDebug() << "EstateView::redraw" << endl;

	// TODO: is this the correct place for name label updates?
	QToolTip::remove(this);
	QToolTip::add(this, m_estate->name());
	lname->setText(m_estate->name());

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
	if (m_recreateQuartz)
	{
		kdDebug() << "EstateView::paintEvent recreating quartz blocks" << endl;
		if (m_quartzBlocks)
			delete m_quartzBlocks;

		if (m_estate->color().isValid())
		{
			kdDebug() << "EstateView::paintEvent drawing quartz blocks" << endl;
			m_quartzBlocks = new KPixmap();
			m_quartzBlocks->resize(25, 18);
			drawQuartzBlocks(m_quartzBlocks, *m_quartzBlocks, m_estate->color().light(60), m_estate->color());
			m_quartzBlocks = rotatePixmap(m_quartzBlocks);
		}

		m_recreateQuartz = false;
		b_recreate = true;
	}

	if (b_recreate)
	{
		kdDebug() << "EstateView::paintEvent recreating pixmap" << endl;
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(Qt::black);
		
		if (atlantikConfig.grayOutMortgaged==true && m_estate->isMortgaged())
			painter.setBrush(atlantik_lgray);
		else if (atlantikConfig.highliteUnowned==true && m_estate->canBeOwned() && !m_ownedByAny)
			painter.setBrush(Qt::white);
		else
			painter.setBrush(m_estate->bgColor());

		painter.drawRect(rect());
        
		// Paint icon only when it exists and fits
		if (icon!=0 && width() > icon->width() && height() > icon->height())
			painter.drawPixmap( (width() - icon->width())/2, (height() - icon->height())/2, *icon);

		if (m_estate->color().isValid())
		{
			int titleHeight = height()/4;
			int titleWidth = width()/4;

			KPixmap* quartzBuffer = new KPixmap;
			if (m_orientation == North || m_orientation == South)
//				quartzBuffer->resize(25, titleHeight-2);
				quartzBuffer->resize(25, 18);
			else
//				quartzBuffer->resize(titleWidth-2, 25);
				quartzBuffer->resize(18, 25);

			QPainter quartzPainter;
			quartzPainter.begin(quartzBuffer, this);

			painter.setBrush(m_estate->color());
			switch(m_orientation)
			{
				case North:
					painter.drawRect(0, 0, width(), titleHeight);

					if (atlantikConfig.quartzEffects && m_quartzBlocks)
					{
						kdDebug() << "EstateView::paintEvent painting quartz on pixmap" << endl;
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(1, 1, *quartzBuffer);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(atlantik_redhotel);
							painter.drawRect(2, 2, (width()/2)-4, (titleHeight)-4);
						}
						else
						{
							// Houses
							painter.setBrush(atlantik_greenhouse);
							int h = (titleHeight)-4, w = (titleWidth)-4;
							for( int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2+(i*(w+2)), 2, w, h);
						}
					}
					break;
				case South:
					painter.drawRect(0, height()-(titleHeight), width(), titleHeight);

					if (atlantikConfig.quartzEffects && m_quartzBlocks)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(width()-quartzBuffer->width()-1, height()-titleHeight+1, *quartzBuffer);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(atlantik_redhotel);
							painter.drawRect(2, (3*(titleHeight))+2, (width()/2)-4, (titleHeight)-4);
						}
						else
						{
							// Houses
							painter.setBrush(atlantik_greenhouse);
							int h = (titleHeight)-4, w = (titleWidth)-4;
							for( int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2+(i*(w+2)), (3*(titleHeight))+2, w, h);
						}
					}
					break;
				case West:
					painter.drawRect(0, 0, titleWidth, height());

					if (atlantikConfig.quartzEffects && m_quartzBlocks)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(1, height()-quartzBuffer->height()-1, *quartzBuffer);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(atlantik_redhotel);
							painter.drawRect(2, 2, (titleWidth)-4, (height()/2)-4);
						}
						else
						{
							// Houses
							painter.setBrush(atlantik_greenhouse);
							int h = (titleHeight)-4, w = (titleWidth)-4;
							for( int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2, 2+(i*(h+2)), w, h);
						}
					}
					break;
				case East:
					painter.drawRect(width()-(titleWidth), 0, titleWidth, height());

					if (atlantikConfig.quartzEffects && m_quartzBlocks)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(width()-quartzBuffer->width()-1, 1, *quartzBuffer);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(atlantik_redhotel);
							painter.drawRect((3*(titleWidth))+2, 2, (titleWidth)-4, (height()/2)-4);
						}
						else
						{
							// Houses
							painter.setBrush(atlantik_greenhouse);
							int h = (titleHeight)-4, w = (titleWidth)-4;
							for( int i=0 ; i < m_estate->houses() ; i++ )
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
		rmbMenu->insertTitle(m_estate->name());
		if (m_estate->isMortgaged())
			rmbMenu->insertItem(i18n("Unmortgage"), 0);
		else
			rmbMenu->insertItem(i18n("Mortgage"), 0);

		if (m_estate->houses()>=4)
			rmbMenu->insertItem(i18n("Build hotel"), 1);
		else
			rmbMenu->insertItem(i18n("Build house"), 1);

		if (m_estate->houses()==5)
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
			if (!(m_estate->canToggleMortgage()))
				rmbMenu->setItemEnabled(0, false);

			// TODO: can_build/can_sell monopd support
			// Empty? Not selling any houses
			if (!m_estate->houses())
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
#warning emit action signals here
//		emit estateToggleMortgage(m_id);
		break;

	case 1:
//		emit estateHouseBuy(m_id);
		break;

	case 2:
//		emit estateHouseSell(m_id);
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
