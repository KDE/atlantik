#ifndef ATLANTIK_PORTFOLIOVIEW_H
#define ATLANTIK_PORTFOLIOVIEW_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qmap.h>

#include "portfolioestate.h"

class Player;

class PortfolioView : public QWidget
{
Q_OBJECT

	public:
		PortfolioView(Player *player, QWidget *parent, const char *name = 0);
		void setOwned(int, bool);

	protected:
		void paintEvent(QPaintEvent *);
	
private slots:
	void playerChanged();

	private:
		Player *m_player;
		QPixmap *qpixmap;
		bool b_recreate;
		QLabel *m_nameLabel, *m_moneyLabel;
		QMap<int, PortfolioEstate*> portfolioMap;
};

#endif
