#ifndef ATLANTIK_TOKEN_H
#define ATLANTIK_TOKEN_H

#include <qwidget.h>
#include <qpixmap.h>

class Player;
class Estate;
class EstateView;
class AtlantikBoard;

class Token : public QWidget
{
Q_OBJECT

	public:
		Token (Player *player, AtlantikBoard *parent, const char *name = 0);
		Player *player();
		void setLocation(EstateView *estateView, bool confirm = true);
		EstateView *location() { return m_location; }
		void setDestination(EstateView *estateView);
		EstateView *destination() { return m_destination; }
		void updateGeometry();

	private slots:
		void playerChanged();

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

signals:
	void tokenConfirmation(Estate *);

private:
		Player *m_player;
		EstateView *m_location, *m_destination;
		AtlantikBoard *m_parentBoard;
		bool b_recreate;
		QPixmap *qpixmap;
};

#endif
