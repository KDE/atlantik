#include <qtooltip.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwmatrix.h>
#include <qpopupmenu.h>

#warning remove iostream output
#include <iostream.h>

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
	m_owned = false;
	m_mortgaged = false;
	m_color = color;

	setBackgroundMode(NoBackground); // avoid flickering

	b_recreate = true;
	qpixmap = 0;

/*
	lname = new QLabel(this);
	lname->setAlignment(Qt::AlignLeft);
	lname->setMinimumSize(lname->sizeHint());
	lname->setMaximumWidth(width());
	lname->setMaximumHeight(15);
	lname->hide();
*/

	pe = 0;
	icon = initIcon(_icon);

	setName("");
	setHouses(0);
	setOwned(false);

	QToolTip::add(this, estatename);
}

QPixmap *EstateView::initIcon(QString name)
{
	if (name.isNull())
		return 0;

	QPixmap *icon = new QPixmap(locate("data", "kmonop/pics/" + name));
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
	*icon = icon->xForm(m);
	return icon;
}

void EstateView::setName(const char *n)
{
	estatename.setLatin1(n, strlen(n));
	QToolTip::remove(this);
	QToolTip::add(this, estatename);
/*
	lname->setText(n);
*/
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

void EstateView::setOwned(bool _o)
{
	m_owned = _o;
	updatePE();
}

bool EstateView::mortgaged() { return m_mortgaged; }
bool EstateView::owned() { return m_owned; }

void EstateView::updatePE()
{
	// Don't show a when a property is not unowned, cannot be owned at all
	// or when the user has configured KMonop not to show them.
	if (m_owned || !m_canBeOwned || kmonopConfig.indicateUnowned==false)
	{
		delete pe;
		pe = 0;
	}
	else
	{
		if (pe==0)
		{
			// Display a coloured portfolioestate in the center to indicate
			// property is for sale
			pe = new PortfolioEstate(this);
			pe->setColor(m_color);
			pe->setOwned(true);
			centerPortfolioEstate();
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

void EstateView::centerPortfolioEstate()
{
	if (pe!=0)
		pe->setGeometry((width() - pe->width())/2, (height() - pe->height())/2, pe->width(), pe->height());
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
		else if (kmonopConfig.highliteUnowned==true && m_canBeOwned && !m_owned)
			painter.setBrush(Qt::white);
		else
			painter.setBrush(kmonop_greenbg);

		painter.drawRect(rect());
        
		// Paint icon only when it exists and fits
		if (icon!=0 && width() > icon->width() && height() > icon->height())
			painter.drawPixmap( (width() - icon->width())/2, (height() - icon->height())/2, *icon);

		if (m_color.isValid())
		{
			painter.setBrush(m_color);
			switch(m_orientation)
			{
				case North:
					painter.drawRect(0, 0, width(), height()/4);
//					painter.setPen(Qt::black);
//					painter.setFont(QFont("helvetica", 10));
//					painter.drawText(0, height()/4, width(), height()/2, (Qt::AlignHCenter | Qt::AlignTop), estatename, estatename.length());
					if (m_houses > 0)
					{
						if (m_houses == 5)
						{
							// Hotel
							painter.setBrush(kmonop_redhotel);
							painter.drawRect(2, 2, (width()/2)-4, (height()/4)-4);
						}
						else
						{
							// Houses
							painter.setBrush(kmonop_greenhouse);
							int h = (height()/4)-4, w = (width()/4)-4;
							for( int i=0 ; i< m_houses ; i++ )
								painter.drawRect(2+(i*(w+2)), 2, w, h);
						}
					}
					break;
				case South:
					painter.drawRect(0, height()-(height()/4), width(), height()/4);
					if (m_houses > 0)
					{
						if (m_houses == 5)
						{
							// Hotel
							painter.setBrush(kmonop_redhotel);
							painter.drawRect(2, (3*(height()/4))+2, (width()/2)-4, (height()/4)-4);
						}
						else
						{
							// Houses
							painter.setBrush(kmonop_greenhouse);
							int h = (height()/4)-4, w = (width()/4)-4;
							for( int i=0 ; i< m_houses ; i++ )
								painter.drawRect(2+(i*(w+2)), (3*(height()/4))+2, w, h);
						}
					}
					break;
				case West:
					painter.drawRect(0, 0, width()/4, height());
					if (m_houses > 0)
					{
						if (m_houses == 5)
						{
							// Hotel
							painter.setBrush(kmonop_redhotel);
							painter.drawRect(2, 2, (width()/4)-4, (height()/2)-4);
						}
						else
						{
							// Houses
							painter.setBrush(kmonop_greenhouse);
							int h = (height()/4)-4, w = (width()/4)-4;
							for( int i=0 ; i< m_houses ; i++ )
								painter.drawRect(2, 2+(i*(h+2)), w, h);
						}
					}
					break;
				case East:
					painter.drawRect(width()-(width()/4), 0, width()/4, height());
					if (m_houses > 0)
					{
						if (m_houses == 5)
						{
							// Hotel
							painter.setBrush(kmonop_redhotel);
							painter.drawRect((3*(width()/4))+2, 2, (width()/4)-4, (height()/2)-4);
						}
						else
						{
							// Houses
							painter.setBrush(kmonop_greenhouse);
							int h = (height()/4)-4, w = (width()/4)-4;
							for( int i=0 ; i< m_houses ; i++ )
								painter.drawRect((3*(width()/4))+2, 2+(i*(h+2)), w, h);
						}
					}
					break;
			}
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

		if (!m_owned)
		{
			rmbMenu->setItemEnabled(0, false);
			rmbMenu->setItemEnabled(1, false);
			rmbMenu->setItemEnabled(2, false);
		}
		else if (m_mortgaged || m_houses==5)
			rmbMenu->setItemEnabled(1, false);
		else if (m_houses==0)
			rmbMenu->setItemEnabled(2, false);

		connect(rmbMenu, SIGNAL(activated(int)), this, SLOT(slotMenuAction(int)));
//		rmbMenu->exec(QPoint(geometry.x(), geometry.y()));
//		rmbMenu->exec(QPoint(pos()));

		QPoint g = QCursor::pos();
//		if (rmbMenu->height() < g.y())
//			rmbMenu->exec(QPoint( g.x(), g.y() - rmbMenu->height()));
//		else
			rmbMenu->exec(g);
	}
}

void EstateView::slotResizeAftermath()
{
	centerPortfolioEstate();
}

void EstateView::slotMenuAction(int item)
{
	QString cmd, loc;
	loc.setNum(m_id);

	cout << "slotMenu " << item << endl;
	switch (item)
	{
		case 0:
			if (mortgaged())
				cmd.setLatin1(".u");
			else
				cmd.setLatin1(".m");
			break;

		case 1:
			cmd.setLatin1(".bh");
			break;

		case 2:
			cmd.setLatin1(".s");
			break;
			
		default:
			return;
	}
	cmd.append(loc);
	gameNetwork->writeData(cmd.latin1());
}
