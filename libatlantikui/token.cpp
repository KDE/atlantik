#include <qpainter.h>
#include <qfont.h>

#include <kdebug.h>

#include <kstandarddirs.h>
#include <kglobalsettings.h>

#include "token.moc"
#include "player.h"
#include "board.h"
#include "estateview.h"

Token::Token(Player *player, AtlantikBoard *parent, const char *name) : QWidget(parent, name)
{
	setBackgroundMode(NoBackground); // avoid flickering

	m_player = player;
	connect(m_player, SIGNAL(changed(Player *)), this, SLOT(playerChanged()));

	m_parentBoard = parent;
	connect(this, SIGNAL(tokenConfirmation(Estate *)), m_parentBoard, SIGNAL(tokenConfirmation(Estate *)));

	qpixmap = 0;
	b_recreate = true;

	setFixedSize(QSize(26, 26));
	m_location = m_destination = 0;
}

Player *Token::player()
{
	return m_player;
}

void Token::setLocation(EstateView *estateView, bool confirm)
{
	if (m_location != estateView)
	{
		m_location = estateView;
		updateGeometry();

		if (confirm)
			emit tokenConfirmation(m_player->location());
	}
}

void Token::setDestination(EstateView *estateView)
{
	if (m_destination != estateView)
	{
		m_destination = estateView;
		updateGeometry();
	}
	emit tokenConfirmation(m_player->location());
}

void Token::playerChanged()
{
	if (m_player->hasTurn())
		raise();

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
	
	b_recreate = true;
	update();
}

void Token::updateGeometry()
{
	if (!(m_location && m_player))
	{
		hide();
		return;
	}

	int x, y;
	if (m_player->inJail())
	{
		x = m_location->geometry().x() + m_location->width() - width() - 2;
		y = m_location->geometry().y() + 2;
	}
	else
	{
		x = m_location->geometry().center().x() - (width()/2);
		y = m_location->geometry().center().y() - (height()/2);
	}

	kdDebug() << "Token::updateGeometry, x:" << x << " y:" << y << endl;
	setGeometry(x, y, width(), height());
	if (isHidden())
		show();
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
		painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Bold));
		painter.drawText(2, height()-2, m_player->name());

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void Token::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}
