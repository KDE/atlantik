#ifndef ATLANTIK_PLAYER_H
#define ATLANTIK_PLAYER_H

#include <qstring.h>

#include <portfolioview.h>

class Player : public QObject
{
Q_OBJECT

public:
	Player(int playerId);
	int playerId() { return m_playerId; }

	void setLocation(const int estateId) { m_location = estateId; }
	const int location() { return m_location; }
	void setIsSelf(const bool isSelf) { m_isSelf = isSelf; }
	const bool isSelf() { return m_isSelf; }
	void setName(const QString _n);
	const QString name() { return m_name; }
	void setMoney(const QString _m);
	void setView(PortfolioView *_pfv) { m_portfolioView = _pfv; }

signals:
	void changed();

private:
	int m_playerId, m_location;
	bool m_isSelf;
	QString m_name, m_money;
	PortfolioView *m_portfolioView;
};

#endif
