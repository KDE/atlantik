#ifndef ATLANTIK_ESTATEDETAILS_H
#define ATLANTIK_ESTATEDETAILS_H

#include <qwidget.h>

class QPixmap;

class KPixmap;

class Player;
class Estate;

class EstateDetails : public QWidget
{
Q_OBJECT

	public:
		EstateDetails(Estate *estate, QWidget *parent, const char *name = 0);
		Estate *estate() { return m_estate; }

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

	private:
		Estate *m_estate;
		QPixmap *m_pixmap;
		KPixmap *m_quartzBlocks;
		bool b_recreate, m_recreateQuartz;
};

#endif
