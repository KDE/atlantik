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

#include <qpainter.h>
#include <QPixmap>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QMenu>
#include <QStandardPaths>
#include <QFontDatabase>
#include <QPaintEvent>
#include <QTransform>

#include <klocalizedstring.h>

#include <player.h>
#include <estate.h>

#include "portfolioestate.h"

#include "estateview.h"

EstateView::EstateView(Estate *estate, EstateOrientation orientation, const QString &_icon, bool indicateUnowned,
                       bool highlightUnowned, bool darkenMortgaged, bool quartzEffects, QWidget *parent)
	: QWidget(parent)
	, m_estate(estate)
	, qpixmap(nullptr)
	, icon(nullptr)
	, m_quartzBlocks(nullptr)
	, m_pe(nullptr)
	, m_allowEstateSales(false)
	, m_indicateUnowned(indicateUnowned)
	, m_highlightUnowned(highlightUnowned)
	, m_darkenMortgaged(darkenMortgaged)
	, m_quartzEffects(quartzEffects)
	, b_recreate(true)
	, m_recreateQuartz(true)
	, m_orientation(orientation)
	, m_estateColor(m_estate->color())
{
        setAttribute(Qt::WA_NoSystemBackground, true);

	updatePE();

	loadIcon(_icon);

	updateToolTip();
}

EstateView::~EstateView()
{
	delete qpixmap;
	delete icon;
	delete m_quartzBlocks;
	delete m_pe;
}

void EstateView::updateToolTip()
{
		QString toolTip = m_estate->name();
		if ( m_estate->isOwned() )
		{
			toolTip.append( QLatin1Char('\n') + i18n("Owner: %1", m_estate->owner()->name() ) );
			if ( m_estate->isMortgaged() )
				toolTip.append( QLatin1Char('\n') + i18n("Unmortgage Price: %1", m_estate->unmortgagePrice() ) );
		     	else
		     		toolTip.append( QLatin1Char('\n') + i18n("Mortgage Value: %1", m_estate->mortgagePrice() ) );
			if ( m_estate->canSellHouses() )
				toolTip.append( QLatin1Char('\n') + i18n("House Value: %1", m_estate->houseSellPrice() ) );
			if ( m_estate->canBuyHouses() )
				toolTip.append( QLatin1Char('\n') + i18n("House Price: %1", m_estate->housePrice() ) );
		}
		else if ( m_estate->canBeOwned() )
			toolTip.append( QLatin1Char('\n') + i18n("Price: %1", m_estate->price() ) );
		else if ( m_estate->money() )
			toolTip.append( QLatin1Char('\n') + i18n("Money: %1", m_estate->money() ) );

		this->setToolTip( toolTip );
}

void EstateView::loadIcon(const QString &_icon)
{
	m_estateIcon = QString();
	delete icon;
	icon = nullptr;
	if (_icon.isEmpty())
		return;
	const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("atlantik/pics/") + _icon);
	if (path.isEmpty())
		return;
	icon = new QPixmap(path);
	if (icon->isNull())
	{
		delete icon;
		icon = nullptr;
	}
	else
	{
		icon = rotatePixmap(icon);
		m_estateIcon = _icon;
	}
}

void EstateView::setViewProperties(bool indicateUnowned, bool highlightUnowned, bool darkenMortgaged, bool quartzEffects)
{
	if (m_indicateUnowned != indicateUnowned)
	{
		m_indicateUnowned = indicateUnowned;
		b_recreate = true;
		updatePE();
	}

	if (m_highlightUnowned != highlightUnowned)
	{
		m_highlightUnowned = highlightUnowned;
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
		m_recreateQuartz = true;
	}

	if (b_recreate || m_recreateQuartz)
		update();
}

QPixmap *EstateView::rotatePixmap(QPixmap *p) const
{
	if (p==nullptr || p->isNull())
		return nullptr;

	QTransform m;

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
	*p = p->transformed(m);
	return p;
}

void EstateView::updatePE()
{
	// Don't show a when a property is not unowned, cannot be owned at all
	// or when the user has configured Atlantik not to show them.
	if (m_estate->isOwned() || !m_estate->canBeOwned() || m_indicateUnowned==false)
	{
		delete m_pe;
		m_pe = nullptr;
	}
	else if (!m_pe)
	{
		// Display a coloured portfolioestate to indicate property is
		// for sale
		m_pe = new QPixmap(PortfolioEstate::drawPixmap(m_estate, nullptr, true));
	}
}

void EstateView::setAllowEstateSales(bool allow)
{
	m_allowEstateSales = allow;
}

void EstateView::estateChanged()
{
	updateToolTip();
	if (m_estate->icon() != m_estateIcon)
		loadIcon(m_estate->icon());

	if (m_estate->color() != m_estateColor)
		m_recreateQuartz = true;
	b_recreate = true;

	m_estateColor = m_estate->color();

	update();
	updatePE();
}

void EstateView::paintEvent(QPaintEvent *e)
{
	m_titleHeight = height()/4;
	m_titleWidth = width()/4;

	if (m_recreateQuartz)
	{
		if (m_quartzBlocks)
		{
			delete m_quartzBlocks;
			m_quartzBlocks = nullptr;
		}

		if (m_quartzEffects && m_estateColor.isValid())
		{
			if (m_orientation == North || m_orientation == South)
				m_quartzBlocks = new QPixmap(25, m_titleHeight-2);
			else
				m_quartzBlocks = new QPixmap(25, m_titleWidth-2);

			drawQuartzBlocks(m_quartzBlocks, m_estateColor.lighter(60), m_estateColor);
			m_quartzBlocks = rotatePixmap(m_quartzBlocks);
		}

		m_recreateQuartz = false;
		b_recreate = true;
	}

	if (b_recreate)
	{
		if (!qpixmap || qpixmap->size() != size())
		{
			delete qpixmap;
			qpixmap = new QPixmap(width(), height());
		}

		QColor greenHouse(0, 255, 0);
		QColor redHotel(255, 51, 51);
		QPainter painter;
		painter.begin( qpixmap );

		painter.setPen(Qt::black);

		if (m_darkenMortgaged==true && m_estate->isMortgaged())
			painter.setBrush(m_estate->bgColor().lighter(10));
		else if (m_highlightUnowned==true && m_estate->canBeOwned() && !m_estate->isOwned())
			painter.setBrush(m_estate->bgColor().lighter(190));
		else
			painter.setBrush(m_estate->bgColor());

		painter.drawRect(rect().adjusted(0, 0, -1, -1));

		// Paint icon only when it exists and fits
		if (icon!=nullptr && width() > icon->width() && height() > icon->height())
			painter.drawPixmap( (width() - icon->width())/2, (height() - icon->height())/2, *icon);

		if (m_estateColor.isValid())
		{
			painter.setBrush(m_estateColor);
			switch(m_orientation)
			{
				case North:
					painter.drawRect(0, 0, width()-1, m_titleHeight-1);

					if (m_quartzEffects && m_quartzBlocks)
					{
						painter.drawPixmap(1, 1, *m_quartzBlocks);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(redHotel);
							painter.drawRect(2, 2, (width()/2)-4-1, (m_titleHeight)-4-1);
						}
						else
						{
							// Houses
							painter.setBrush(greenHouse);
							int h = (m_titleHeight)-4, w = (m_titleWidth)-4;
							for( unsigned int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2+(i*(w+2)), 2, w-1, h-1);
						}
					}
					break;
				case South:
					painter.drawRect(0, height()-(m_titleHeight), width()-1, m_titleHeight-1);

					if (m_quartzEffects && m_quartzBlocks)
					{
						painter.drawPixmap(width()-m_quartzBlocks->width()-1, height()-m_titleHeight+1, *m_quartzBlocks);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(redHotel);
							painter.drawRect(2, (3*(m_titleHeight))+2, (width()/2)-4-1, (m_titleHeight)-4-1);
						}
						else
						{
							// Houses
							painter.setBrush(greenHouse);
							int h = (m_titleHeight)-4, w = (m_titleWidth)-4;
							for( unsigned int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2+(i*(w+2)), (3*(m_titleHeight))+2, w-1, h-1);
						}
					}
					break;
				case West:
					painter.drawRect(0, 0, m_titleWidth-1, height()-1);

					if (m_quartzEffects && m_quartzBlocks)
					{
						painter.drawPixmap(1, height()-m_quartzBlocks->height()-1, *m_quartzBlocks);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(redHotel);
							painter.drawRect(2, 2, (m_titleWidth)-4-1, (height()/2)-4-1);
						}
						else
						{
							// Houses
							painter.setBrush(greenHouse);
							int h = (m_titleHeight)-4, w = (m_titleWidth)-4;
							for( unsigned int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect(2, 2+(i*(h+2)), w-1, h-1);
						}
					}
					break;
				case East:
					painter.drawRect(width()-(m_titleWidth), 0, m_titleWidth-1, height()-1);

					if (m_quartzEffects && m_quartzBlocks)
					{
						painter.drawPixmap(width()-m_quartzBlocks->width()-1, 1, *m_quartzBlocks);
					}

					if (m_estate->houses() > 0)
					{
						if (m_estate->houses() == 5)
						{
							// Hotel
							painter.setBrush(redHotel);
							painter.drawRect((3*(m_titleWidth))+2, 2, (m_titleWidth)-4-1, (height()/2)-4-1);
						}
						else
						{
							// Houses
							painter.setBrush(greenHouse);
							int h = (m_titleHeight)-4, w = (m_titleWidth)-4;
							for( unsigned int i=0 ; i < m_estate->houses() ; i++ )
								painter.drawRect((3*(m_titleWidth))+2, 2+(i*(h+2)), w-1, h-1);
						}
					}
					break;
			}
		}

		const QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
		painter.setFont(font);
		QString estateName = m_estate->name();
		QFontMetrics fm( font );
		if ( m_estateColor.isValid() && ( m_orientation == West || m_orientation == East ) )
		{
			estateName = fm.elidedText( m_estate->name(), Qt::ElideRight, 3*width()/4 );
		}
		else
			estateName = fm.elidedText( m_estate->name(), Qt::ElideRight,width() );
		if (m_estateColor.isValid() && m_orientation == West)
                        painter.drawText( width()/4 + 2, height()/2, estateName );
		else
			painter.drawText(2, height()/2, estateName );

		if (m_pe)
		{
			int x = (m_orientation == West ? (width()-2 - m_pe->width()) : 2);
			int y = (m_orientation == North ? (height()-2 - m_pe->height()) : 2);
			painter.drawPixmap(x, y, *m_pe);
		}

		b_recreate = false;
	}
	QPainter painter(this);
	painter.drawPixmap(e->rect(), *qpixmap, e->rect());
}

void EstateView::resizeEvent(QResizeEvent *)
{
	m_recreateQuartz = true;
	b_recreate = true;
}

void EstateView::contextMenuEvent(QContextMenuEvent *e)
{
	if (m_estate->isOwned())
	{
		QMenu *rmbMenu = new QMenu(this);
		rmbMenu->setTitle(m_estate->name());

		if (m_estate->isOwnedBySelf())
		{
			Player *player = m_estate->owner();

			// Mortgage toggle
			if (m_estate->isMortgaged())
			{
				QAction *act = rmbMenu->addAction(i18n("Unmortgage"), this, SLOT(slotToggleMortgage()));
				if (!m_estate->canToggleMortgage() || player->hasDebt())
					act->setEnabled(false);
			}
			else
			{
				QAction *act = rmbMenu->addAction(i18n("Mortgage"), this, SLOT(slotToggleMortgage()));
				if (!m_estate->canToggleMortgage())
					act->setEnabled(false);
			}

			QAction *act = nullptr;
			// Estate construction
			if (m_estate->houses()>=4)
				act = rmbMenu->addAction(i18n("Build Hotel"), this, SLOT(slotHouseBuy()));
			else
				act = rmbMenu->addAction(i18n("Build House"), this, SLOT(slotHouseBuy()));

			if (!m_estate->canBuyHouses() || player->hasDebt())
				act->setEnabled(false);

			// Estate destruction
			if (m_estate->houses()==5)
				act = rmbMenu->addAction(i18n("Sell Hotel"), this, SLOT(slotHouseSell()));
			else
				act = rmbMenu->addAction(i18n("Sell House"), this, SLOT(slotHouseSell()));

			if (!(m_estate->canSellHouses()))
				act->setEnabled(false);

			// Estate sell
			act = rmbMenu->addAction(i18n("Sell"), this, SLOT(slotSell()));
			if (!m_allowEstateSales)
				act->setEnabled(false);
		}
		else
		{
			// Request trade
			if (Player *player = m_estate->owner())
				rmbMenu->addAction(i18n("Request Trade with %1", player->name()), this, SLOT(slotNewTrade()));
		}

		rmbMenu->exec(e->globalPos());
		delete rmbMenu;
	}
}

void EstateView::mousePressEvent(QMouseEvent *e)
{
	if (e->button()==Qt::LeftButton)
		Q_EMIT LMBClicked(m_estate);
}

void EstateView::slotToggleMortgage()
{
	Q_EMIT estateToggleMortgage(m_estate);
}

void EstateView::slotHouseBuy()
{
	Q_EMIT estateHouseBuy(m_estate);
}

void EstateView::slotHouseSell()
{
	Q_EMIT estateHouseSell(m_estate);
}

void EstateView::slotSell()
{
	Q_EMIT estateSell(m_estate);
}

void EstateView::slotNewTrade()
{
	Q_EMIT newTrade(m_estate->owner());
}

// Kudos to Gallium <gallium@kde.org> for writing the Quartz KWin style and
// letting me use the ultra slick algorithm!
void EstateView::drawQuartzBlocks(QPixmap *pi, const QColor &c1, const QColor &c2)
{
	QPainter px;

	if (pi==nullptr || pi->isNull())
		return;

	px.begin(pi);

	QLinearGradient gradient(0, 0, pi->width(), 0);
	gradient.setColorAt(0, c1);
	gradient.setColorAt(1, c2);
	px.fillRect(pi->rect(), gradient);

	px.fillRect( 2, 1, 3, 3, c1.lighter(120) );
	px.fillRect( 2, 5, 3, 3, c1 );
	px.fillRect( 2, 9, 3, 3, c1.lighter(110) );
	px.fillRect( 2, 13, 3, 3, c1 );

	px.fillRect( 6, 1, 3, 3, c1.lighter(110) );
	px.fillRect( 6, 5, 3, 3, c2.lighter(110) );
	px.fillRect( 6, 9, 3, 3, c1.lighter(120) );
	px.fillRect( 6, 13, 3, 3, c2.lighter(130) );

	px.fillRect( 10, 5, 3, 3, c1.lighter(110) );
	px.fillRect( 10, 9, 3, 3, c2.lighter(120) );
	px.fillRect( 10, 13, 3, 3, c2.lighter(150) );

	px.fillRect( 14, 1, 3, 3, c1.darker(110) );
	px.fillRect( 14, 9, 3, 3, c2.lighter(120) );
	px.fillRect( 14, 13, 3, 3, c1.darker(120) );

	px.fillRect( 18, 5, 3, 3, c1.lighter(110) );
	px.fillRect( 18, 13, 3, 3, c1.darker(110) );

	px.fillRect( 22, 9, 3, 3, c2.lighter(120));
	px.fillRect( 22, 13, 3, 3, c2.lighter(110) );
}

#include "moc_estateview.cpp"
