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
	public:
		PortfolioView(Player *parentPlayer, QWidget *parent, const char *name = 0);
		void updateName();
		void setMoney(const char *);
		void setOwned(int, bool);
		void setHasTurn(bool);

	protected:
		void paintEvent(QPaintEvent *);
	
	private:
		Player *m_parentPlayer;
		QPixmap *qpixmap;
		bool b_recreate;
		QLabel *lname, *lmoney;
//		PortfolioEstate *estate[40];
		bool myHasTurn;
		QMap<int, PortfolioEstate*> portfolioMap;
};

#endif
