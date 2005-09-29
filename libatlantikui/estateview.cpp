// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <qtooltip.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwmatrix.h>
#include <qcursor.h>

#include <kdebug.h>
#include <kdeversion.h>
#include <kdialogbase.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpixmapeffect.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kstringhandler.h>

#include <player.h>
#include <estate.h>

#include "estateview.moc"
#include "config.h"

EstateView::EstateView(Estate *estate, EstateOrientation orientation, const QString &_icon, bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects, QWidget *parent, const char *name) : QWidget(parent, name, WResizeNoErase)
{
	m_estate = estate;
	m_orientation = orientation;

	m_indicateUnowned = indicateUnowned;
	m_highliteUnowned = highliteUnowned;
	m_darkenMortgaged = darkenMortgaged;
	m_quartzEffects = quartzEffects;

	setBackgroundMode(NoBackground); // avoid flickering

	qpixmap = 0;
	b_recreate = true;

	m_quartzBlocks = 0;	
	m_recreateQuartz = true;

	pe = 0;
	updatePE();

	icon = new QPixmap(locate("data", "atlantik/pics/" + _icon));
	icon = rotatePixmap(icon);

	updateToolTip();
}

void EstateView::updateToolTip()
{
	QToolTip::remove(this);

	if ( m_estate )
	{
		QString toolTip = m_estate->name();
		if ( m_estate->isOwned() )
		{
			toolTip.append( "\n" + i18n("Owner: %1").arg( m_estate->owner()->name() ) );
			if ( m_estate->isMortgaged() )
				toolTip.append( "\n" + i18n("Unmortgage Price: %1").arg( m_estate->unmortgagePrice() ) );
		     	else
		     		toolTip.append( "\n" + i18n("Mortgage Value: %1").arg( m_estate->mortgagePrice() ) );	
			if ( m_estate->canSellHouses() )
				toolTip.append( "\n" + i18n("House Value: %1").arg( m_estate->houseSellPrice() ) );
			if ( m_estate->canBuyHouses() )
				toolTip.append( "\n" + i18n("House Price: %1").arg( m_estate->housePrice() ) );
		}
		else if ( m_estate->canBeOwned() )
			toolTip.append( "\n" + i18n("Price: %1").arg( m_estate->price() ) );
		else if ( m_estate->money() )
			toolTip.append( "\n" + i18n("Money: %1").arg( m_estate->money() ) );

		QToolTip::add( this, toolTip );
	}
}

void EstateView::setViewProperties(bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects)
{
	if (m_indicateUnowned != indicateUnowned)
	{
		m_indicateUnowned = indicateUnowned;
		b_recreate = true;
		updatePE();
	}

	if (m_highliteUnowned != highliteUnowned)
	{
		m_highliteUnowned = highliteUnowned;
		b_recreate = true;
	}

	if (m_darkenMortgaged != darkenMortgaged)
	{
		m_darkenMortgaged = darkenMortgaged;
		b_recreate = true;
	}

	if (m_quartzEffects != quartzEffects)
	{
		m_quartzEffects = quartzEffects;
		b_recreate = true;
//		m_recreateQuartz = true;
	}

	if (b_recreate || m_recreateQuartz)
		update();
}

QPixmap *EstateView::rotatePixmap(QPixmap *p)
{
	if (p==0 || p->isNull())
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
		default:;
	}
	*p = p->xForm(m);
	return p;
}

KPixmap *EstateView::rotatePixmap(KPixmap *p)
{
	if (p==0 || p->isNull())
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
		default:;
	}
	*p = p->xForm(m);
	return p;
}

void EstateView::updatePE()
{
	// Don't show a when a property is not unowned, cannot be owned at all
	// or when the user has configured Atlantik not to show them.
	if (m_estate->isOwned() || !m_estate->canBeOwned() || m_indicateUnowned==false)
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
			pe = new PortfolioEstate(m_estate, 0, true, this, "board-portfolioestate");
			repositionPortfolioEstate();

			pe->show();
		}
		else if (!pe->isVisible())
			pe->show();
	}
}

void EstateView::estateChanged()
{
	updateToolTip();

	b_recreate = true;
	m_recreateQuartz = true;

	update();
	updatePE();
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
	m_titleHeight = height()/4;
	m_titleWidth = width()/4;

	if (m_recreateQuartz)
	{
		if (m_quartzBlocks)
		{
			delete m_quartzBlocks;
			m_quartzBlocks = 0;
		}

		if (m_estate->color().isValid())
		{
			m_quartzBlocks = new KPixmap();

			if (m_orientation == North || m_orientation == South)
				m_quartzBlocks->resize(25, m_titleHeight-2);
			else
				m_quartzBlocks->resize(25, m_titleWidth-2);

			drawQuartzBlocks(m_quartzBlocks, *m_quartzBlocks, m_estate->color().light(60), m_estate->color());
			m_quartzBlocks = rotatePixmap(m_quartzBlocks);
		}

		m_recreateQuartz = false;
		b_recreate = true;
	}

	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QColor greenHouse(0, 255, 0);
		QColor redHotel(255, 51, 51);
		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(Qt::black);
		
		if (m_darkenMortgaged==true && m_estate->isMortgaged())
			painter.setBrush(m_estate->bgColor().light(10));
		else if (m_highliteUnowned==true && m_estate->canBeOwned() && !m_estate->isOwned())
			painter.setBrush(m_estate->bgColor().light(190));
		else
			painter.setBrush(m_estate->bgColor());

		painter.drawRect(rect());
        
		// Paint icon only when it exists and fits
		if (icon!=0 && width() > icon->width() && height() > icon->height())
			painter.drawPixmap( (width() - icon->width())/2, (height() - icon->height())/2, *icon);

		if (m_estate->color().isValid())
		{
			KPixmap* quartzBuffer = new KPixmap;
			if (m_orientation == North || m_orientation == South)
				quartzBuffer->resize(25, m_titleHeight-2);
			else
				quartzBuffer->resize(m_titleWidth-2, 25);

			QPainter quartzPainter;
			quartzPainter.begin(quartzBuffer, this);

			painter.setBrush(m_estate->color());
			switch(m_orientation)
			{
				case North:
					painter.drawRect(0, 0, width(), m_titleHeight);

					if (m_quartzEffects && m_quartzBlocks)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(1, 1, *quartzBuffer);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(redHotel);
							painter.drawRect(2, 2, (width()/2)-4, (m_titleHeight)-4);
						}
						else
						{
							// Houses
							painter.setBrush(greenHouse);
							int h = (m_titleHeight)-4, w = (m_titleWidth)-4;
							for( unsigned int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2+(i*(w+2)), 2, w, h);
						}
					}
					break;
				case South:
					painter.drawRect(0, height()-(m_titleHeight), width(), m_titleHeight);

					if (m_quartzEffects && m_quartzBlocks)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(width()-quartzBuffer->width()-1, height()-m_titleHeight+1, *quartzBuffer);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(redHotel);
							painter.drawRect(2, (3*(m_titleHeight))+2, (width()/2)-4, (m_titleHeight)-4);
						}
						else
						{
							// Houses
							painter.setBrush(greenHouse);
							int h = (m_titleHeight)-4, w = (m_titleWidth)-4;
							for( unsigned int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2+(i*(w+2)), (3*(m_titleHeight))+2, w, h);
						}
					}
					break;
				case West:
					painter.drawRect(0, 0, m_titleWidth, height());

					if (m_quartzEffects && m_quartzBlocks)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(1, height()-quartzBuffer->height()-1, *quartzBuffer);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(redHotel);
							painter.drawRect(2, 2, (m_titleWidth)-4, (height()/2)-4);
						}
						else
						{
							// Houses
							painter.setBrush(greenHouse);
							int h = (m_titleHeight)-4, w = (m_titleWidth)-4;
							for( unsigned int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2, 2+(i*(h+2)), w, h);
						}
					}
					break;
				case East:
					painter.drawRect(width()-(m_titleWidth), 0, m_titleWidth, height());

					if (m_quartzEffects && m_quartzBlocks)
					{
						quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
						painter.drawPixmap(width()-quartzBuffer->width()-1, 1, *quartzBuffer);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(redHotel);
							painter.drawRect((3*(m_titleWidth))+2, 2, (m_titleWidth)-4, (height()/2)-4);
						}
						else
						{
							// Houses
							painter.setBrush(greenHouse);
							int h = (m_titleHeight)-4, w = (m_titleWidth)-4;
							for( unsigned int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect((3*(m_titleWidth))+2, 2+(i*(h+2)), w, h);
						}
					}
					break;
			}


			quartzPainter.end();
			delete quartzBuffer;
		}

		QFont font = QFont( KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Normal );
		painter.setFont(font);
		QString estateName = m_estate->name();
#if defined(KDE_MAKE_VERSION)
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
                if ( m_estate->color().isValid() && ( m_orientation == West || m_orientation == East ) )
			estateName = KStringHandler::rPixelSqueeze( m_estate->name(), QFontMetrics( font ), 3*width()/4 );
		else
			estateName = KStringHandler::rPixelSqueeze( m_estate->name(), QFontMetrics( font ), width() );
#endif
#endif
		if (m_estate->color().isValid() && m_orientation == West)
                        painter.drawText( width()/4 + 2, height()/2, estateName );
		else
			painter.drawText(2, height()/2, estateName );

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void EstateView::resizeEvent(QResizeEvent *)
{
	m_recreateQuartz = true;
	b_recreate = true;

	QTimer::singleShot(0, this, SLOT(slotResizeAftermath()));
}

void EstateView::mousePressEvent(QMouseEvent *e) 
{
	if (e->button()==RightButton && m_estate->isOwned())
	{
		KPopupMenu *rmbMenu = new KPopupMenu(this);
		rmbMenu->insertTitle(m_estate->name());

		if (m_estate->isOwnedBySelf())
		{
			Player *player = m_estate->owner();

			// Mortgage toggle
			if (m_estate->isMortgaged())
			{
				rmbMenu->insertItem(i18n("Unmortgage"), 0);
				if (!m_estate->canToggleMortgage() || player->hasDebt())
					rmbMenu->setItemEnabled(0, false);
			}
			else
			{
				rmbMenu->insertItem(i18n("Mortgage"), 0);
				if (!m_estate->canToggleMortgage())
					rmbMenu->setItemEnabled(0, false);
			}

			// Estate construction
			if (m_estate->houses()>=4)
				rmbMenu->insertItem(i18n("Build Hotel"), 1);
			else
				rmbMenu->insertItem(i18n("Build House"), 1);

			if (!m_estate->canBuyHouses() || player->hasDebt())
				rmbMenu->setItemEnabled(1, false);

			// Estate destruction
			if (m_estate->houses()==5)
				rmbMenu->insertItem(i18n("Sell Hotel"), 2);
			else
				rmbMenu->insertItem(i18n("Sell House"), 2);

			if (!(m_estate->canSellHouses()))
				rmbMenu->setItemEnabled(2, false);
		}
		else
		{
			// Request trade
			if (Player *player = m_estate->owner())
				rmbMenu->insertItem(i18n("Request Trade with %1").arg(player->name()), 3);
		}

		KPopupMenu *pm = dynamic_cast<KPopupMenu *>(rmbMenu);
		if (pm) {
			connect(pm, SIGNAL(activated(int)), this, SLOT(slotMenuAction(int)));
		}
		QPoint g = QCursor::pos();
		rmbMenu->exec(g);
		delete rmbMenu;
	}
	else if (e->button()==LeftButton)
		emit LMBClicked(m_estate);
}

void EstateView::slotResizeAftermath()
{
	repositionPortfolioEstate();
}

void EstateView::slotMenuAction(int item)
{
	switch (item)
	{
	case 0:
		emit estateToggleMortgage(m_estate);
		break;

	case 1:
		emit estateHouseBuy(m_estate);
		break;

	case 2:
		emit estateHouseSell(m_estate);
		break;

	case 3:
		emit newTrade(m_estate->owner());
		break;
	}
}

// Kudos to Gallium <gallium@kde.org> for writing the Quartz KWin style and
// letting me use the ultra slick algorithm!
void EstateView::drawQuartzBlocks(KPixmap *pi, KPixmap &p, const QColor &c1, const QColor &c2)
{
	QPainter px;

	if (pi==0 || pi->isNull())
		return;

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
