#ifndef ATLANTIK_TOKEN_H
#define ATLANTIK_TOKEN_H

#include <qwidget.h>
#include <qpixmap.h>

class Player;

class Token : public QWidget
{
Q_OBJECT

	public:
		Token (Player *player, QWidget *parent, const char *name = 0);
		void setLocation(int);
		int location();
		void setDestination(int);
		int destination();
		void moveTo(int);

	private slots:
		void playerChanged();

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

	private:
		Player *m_player;
		bool b_recreate;
		QPixmap *qpixmap;
		QString myId;
		int myLoc, myDest;
};

#endif
