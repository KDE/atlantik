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
#include <QFile>
#include <QMultiHash>
#include <QMenu>
#include <QApplication>
#include <QStyle>

#include <kglobalsettings.h>
#include <klocalizedstring.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <estategroup.h>
#include <portfolioestate.h>

#include "portfolioview.h"

#include <algorithm>

#define PE_DISTW	4
#define	PE_DISTH	4
#define PE_SPACE	2
#define PE_MARGINW	5
#define PE_MARGINH	2
#define ICONSIZE	48

PortfolioView::PortfolioView(AtlanticCore *core, Player *player, QColor activeColor, QColor inactiveColor, QWidget *parent) : QWidget(parent)
{
	m_atlanticCore = core;
	m_player = player;
	m_activeColor = activeColor;
	m_inactiveColor = inactiveColor;

	qpixmap = 0;
	b_recreate = true;

	QPalette pal = palette();
	pal.setColor(backgroundRole(), Qt::white);
	setPalette(pal);
	setMinimumHeight(ICONSIZE);

	// Init icon
	m_image = 0;
	loadIcon();
}

PortfolioView::~PortfolioView()
{
	clearPortfolio();
	delete m_image;
	delete qpixmap;
}

Player *PortfolioView::player() const
{
	return m_player;
}

void PortfolioView::buildPortfolio()
{
	if ( m_portfolioEstates.count() )
		clearPortfolio();

	// Loop through estate groups in order
	PortfolioEstate *lastPE = 0, *firstPEprevGroup = 0;

	int x = 100, y = 25, marginHint = 5, bottom;
	bottom = ICONSIZE - PE_HEIGHT - marginHint;

	QMultiHash<EstateGroup *, Estate *> groups;

	foreach (Estate *estate, m_atlanticCore->estates())
		groups.insert(estate->estateGroup(), estate);

	foreach (EstateGroup *estateGroup, m_atlanticCore->estateGroups())
	{
		{
			// New group
			lastPE = 0;

			QList<Estate *> estates = groups.values(estateGroup);
			// QMultiHash/QHash tracks multiple values per key
			// in LIFO order
			std::reverse(estates.begin(), estates.end());

			// Loop through estates
			foreach (Estate *estate, estates)
			{
				{
					// Create PE
                                        PortfolioEstate *portfolioEstate = new PortfolioEstate(estate, m_player, false, this );
                                        portfolioEstate->setObjectName( "portfolioestate");
					m_portfolioEstates.append(portfolioEstate);

 					connect(portfolioEstate, SIGNAL(estateClicked(Estate *)), this, SIGNAL(estateClicked(Estate *)));
					if (lastPE)
					{
						x = lastPE->x() + 2;
						y = lastPE->y() + 4;
						if (y > bottom)
							bottom = y;
					}
					else if (firstPEprevGroup)
					{
						x = firstPEprevGroup->x() + PE_WIDTH + 8;
						y = 20 + marginHint;
						firstPEprevGroup = portfolioEstate;
					}
					else
					{
						x = ICONSIZE + marginHint;
						y = 20 + marginHint;
						if (y > bottom)
							bottom = y;
						firstPEprevGroup = portfolioEstate;
					}

					portfolioEstate->setGeometry(x, y, portfolioEstate->width(), portfolioEstate->height());
					portfolioEstate->show();

					lastPE = portfolioEstate;
				}
			}
		}
	}
	setMinimumWidth(x + PE_WIDTH + marginHint);
	int minHeight = bottom + PE_HEIGHT + marginHint;
	if (minHeight > minimumHeight())
		setMinimumHeight(minHeight);
}

void PortfolioView::clearPortfolio()
{
	qDeleteAll(m_portfolioEstates);
	m_portfolioEstates.clear();
}

void PortfolioView::setTokenTheme(const TokenTheme &theme)
{
	m_tokenTheme = theme;
	loadIcon(true);
	b_recreate = true;
	update();
}

void PortfolioView::loadIcon(bool force)
{
	if (m_imageName == m_player->image() && !force)
		return;
	m_imageName = m_player->image();

	delete m_image;
	m_image = 0;

	if (!m_tokenTheme.isValid())
		return;

	QString imageFile;
	if (!m_imageName.isEmpty())
		imageFile = m_tokenTheme.tokenPath(m_imageName);
	if (imageFile.isEmpty())
	{
		m_imageName = m_tokenTheme.fallbackIcon();
		imageFile = m_tokenTheme.tokenPath(m_imageName);
	}

	const QPixmap pix(imageFile);
	if (pix.isNull())
		return;

	QMatrix m;
	m.scale(double(ICONSIZE) / pix.width(), double(ICONSIZE) / pix.height());
	m_image = new QPixmap(pix.transformed(m));
}

void PortfolioView::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		const int marginHint = QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin);
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap);
		painter.initFrom(this);

		painter.setPen(Qt::white);
		painter.setBrush(Qt::white);
		painter.drawRect(rect());

		painter.setPen(m_player->hasTurn() ? m_activeColor : Qt::black);
		painter.setBrush(m_player->hasTurn() ? m_activeColor : Qt::black);
		painter.drawRect(0, 0, width() - 1, 20 - 1);

		if (m_image)
		{
			painter.setPen(Qt::black);
			painter.setBrush(Qt::white);
			painter.drawRect(0, 0, ICONSIZE - 1, ICONSIZE - 1);

			painter.drawPixmap(0, 0, *m_image);
		}

		painter.setPen(Qt::white);
		painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Bold));
		painter.drawText(ICONSIZE + marginHint, 15, m_player->name());

		if ( m_portfolioEstates.count() )
			painter.drawText(width() - 50, 15, QString::number(m_player->money()));
		else
		{
			painter.setPen(Qt::black);
			painter.setBrush(Qt::white);

			painter.setFont(KGlobalSettings::generalFont());
			painter.drawText(ICONSIZE + marginHint, 30, m_player->host());
		}

		b_recreate = false;
	}
	QPainter painter(this);
	painter.drawPixmap(0, 0, *qpixmap);
}

void PortfolioView::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}

void PortfolioView::playerChanged()
{
	loadIcon();

	b_recreate = true;
	update();
}

void PortfolioView::mousePressEvent(QMouseEvent *e)
{
	Player *playerSelf = m_atlanticCore->playerSelf();

	if ( e->button()==Qt::RightButton && (m_player != playerSelf) )
	{
		QMenu rmbMenu(this);
		rmbMenu.setTitle(m_player->name());
		QAction *act;

		if ( m_portfolioEstates.count() )
		{
			// Start trade
			act = rmbMenu.addAction(i18n("Request Trade with %1", m_player->name()));
			connect(act, SIGNAL(triggered()), this, SLOT(slotMenuActionTrade()));
		}
		else
		{
			// Kick player
			act = rmbMenu.addAction(i18n("Boot Player %1 to Lounge", m_player->name()));
			act->setEnabled(m_atlanticCore->selfIsMaster());
			connect(act, SIGNAL(triggered()), this, SLOT(slotMenuActionKick()));
		}

		rmbMenu.exec(e->globalPos());
	}
}

void PortfolioView::slotMenuActionTrade()
{
	emit newTrade(m_player);
}

void PortfolioView::slotMenuActionKick()
{
	emit kickPlayer(m_player);
}
