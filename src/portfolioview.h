#ifndef ATLANTIK_PORTFOLIOVIEW_H
#define ATLANTIK_PORTFOLIOVIEW_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>

#include "portfolioestate.h"

class PortfolioView : public QWidget
{
	public:
		PortfolioView(QWidget *parent, const char *name = 0);
		void setName(const char *);
		void setMoney(const char *);
		void setOwned(int, bool);
		void setHasTurn(bool);

	protected:
		void paintEvent(QPaintEvent *);
	
	private:
		QPixmap *qpixmap;
		bool b_recreate;
		QLabel *lname, *lmoney;
		PortfolioEstate *estate[40];
		bool myHasTurn;
};

#endif
