#ifndef __KMONOP_TOKEN_H__
#define __KMONOP_TOKEN_H__

#include <qwidget.h>
#include <qpixmap.h>

class Token : public QWidget
{
	public:
		Token (QString, QWidget *parent, const char *name = 0);

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

	private:
		bool b_recreate;
		QPixmap *qpixmap;
		QString myId;
};

#endif
