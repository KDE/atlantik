#include <qpainter.h>
#include <qfont.h>

#include <kdebug.h>

#include <kstandarddirs.h>

#include "token.moc"
#include "player.h"
#include "board.h"
#include "estateview.h"

Token::Token(Player *player, AtlantikBoard *parent, const char *name) : QWidget(parent, name)
{
	setBackgroundMode(NoBackground); // avoid flickering

	m_player = player;
	m_parentBoard = parent;

	qpixmap = 0;
	b_recreate = true;

	setFixedSize(QSize(26, 26));
	myId = QString("");
	m_location = m_destination = 0;
}

void Token::setLocation(EstateView *estateView)
{
	if (m_location != estateView)
	{
		m_location = estateView;
		updateGeometry();
	}
}

void Token::setDestination(EstateView *estateView)
{
	if (m_destination != estateView)
	{
		m_destination = estateView;
		updateGeometry();
	}
}

void Token::playerChanged()
{
	kdDebug() << "Token::playerChanged()" << endl;
	if (Estate *estate = m_player->location())
	{
		EstateView *estateView;
		QPtrList<EstateView> estateViews = m_parentBoard->estateViews();
		for (QPtrListIterator<EstateView> it(estateViews); *it; ++it)
		{
			if ((estateView = dynamic_cast<EstateView*>(*it)))
			{
				if (estateView->estate() == estate)
				{
					setLocation(estateView);
					break;
				}
			}
		}
	}
}

void Token::updateGeometry()
{
	kdDebug() << "new geometry for token: " << m_player->location() << endl;
//	setGeometry(100, 100, 125, 125);
	if (m_location)
	{
		show();
	}
	else
	{
		hide();
	}
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
