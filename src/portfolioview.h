#ifndef ATLANTIK_PORTFOLIOVIEW_H
#define ATLANTIK_PORTFOLIOVIEW_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qmap.h>

#include "portfolioestate.h"

class Player;
class Estate;

class PortfolioView : public QWidget
{
Q_OBJECT

	public:
		PortfolioView(Player *player, QWidget *parent, const char *name = 0);
		void addEstateView(Estate *estate);

protected:
//		void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *);

signals:
	void newTrade(int playerId);

private slots:
	void playerChanged();
	void slotMenuAction(int item);

	private:
		Player *m_player;
		QPixmap *qpixmap;
		bool b_recreate;
		QLabel *m_nameLabel, *m_moneyLabel;
		QMap<int, PortfolioEstate*> portfolioEstateMap;
		int x, y, m_lastGroupId;
};

#endif
