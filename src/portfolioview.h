#ifndef __KMONOP_PORTFOLIOVIEW_H__
#define __KMONOP_PORTFOLIOVIEW_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>

class PortfolioView : public QWidget
{
	public:
		PortfolioView(QWidget *parent, const char *name = 0);
		void setName(const char *);
	protected:
		void paintEvent(QPaintEvent *);
	
	private:
		QPixmap *qpixmap;
		bool b_recreate;
		QLabel *lname, *lcash;
		QString name;
		int money;
};

#endif
