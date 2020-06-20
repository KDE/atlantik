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

#ifndef ATLANTIK_PORTFOLIOVIEW_H
#define ATLANTIK_PORTFOLIOVIEW_H

#include <QWidget>
#include <QList>

#include "libatlantikui_export.h"
#include "tokentheme.h"

class QColor;
class QPixmap;
class QString;

class AtlanticCore;
class Player;
class Estate;
class PortfolioEstate;

class LIBATLANTIKUI_EXPORT PortfolioView : public QWidget
{
Q_OBJECT

public:
	PortfolioView(AtlanticCore *core, Player *_player, QColor activeColor, QColor inactiveColor, QWidget *parent);
	~PortfolioView();

	void buildPortfolio();
	void clearPortfolio();

	Player *player() const;

	void setTokenTheme(const TokenTheme &theme);

protected:
	void paintEvent(QPaintEvent *) override;
	void resizeEvent(QResizeEvent *) override;
	void contextMenuEvent(QContextMenuEvent *) override;

Q_SIGNALS:
	void newTrade(Player *player);
	void kickPlayer(Player *player);
	void estateClicked(Estate *);

private Q_SLOTS:
	void playerChanged();
	void slotMenuActionTrade();
	void slotMenuActionKick();

private:
	void loadIcon(bool force = false);

	AtlanticCore *m_atlanticCore;
	Player *m_player;
	QColor m_activeColor, m_inactiveColor;
	QPixmap *qpixmap, *m_image;
	QString m_imageName;
	bool b_recreate;
	QList<PortfolioEstate *> m_portfolioEstates;
	TokenTheme m_tokenTheme;
};

#endif
