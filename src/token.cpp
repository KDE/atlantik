#include <qpainter.h>
#include <qfont.h>

#include <kdebug.h>

#include <kstandarddirs.h>

#include "token.moc"
#include "player.h"

extern QColor atlantik_greenbg;

Token::Token(Player *player, QWidget *parent, const char *name) : QWidget(parent, name)
{
	setBackgroundMode(NoBackground); // avoid flickering

	m_player = player;

	qpixmap = 0;
	b_recreate = true;

	setFixedSize(QSize(26, 26));
	myId = QString("");
	m_location = m_destination = 0;
}

void Token::playerChanged()
{
	kdDebug() << "Token::playerChanged()" << endl;
	kdDebug() << "new geometry for token: " << m_player->location() << endl;
	setGeometry(100, 100, 125, 125);
}

void Token::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(locate("data", "atlantik/pics/token.png"));

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.drawPixmap(0, 0, *qpixmap);

		painter.setPen(Qt::black);
		painter.setFont(QFont("Helvetica", 12, QFont::Bold));
		painter.drawText(2, height()-2, myId);

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void Token::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}
