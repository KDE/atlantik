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

	void setLocation(const int estateId);
	const int location() { return m_location; }
	void setIsSelf(const bool isSelf) { m_isSelf = isSelf; }
	const bool isSelf() { return m_isSelf; }
	void setHasTurn(const bool hasTurn);
	const bool hasTurn() { return m_hasTurn; }
	void setName(const QString _n);
	const QString name() { return m_name; }
	void setMoney(const QString _m);
	const QString money() { return m_money; }
	void update(bool force = false);

signals:
	void changed();

private:
	bool m_changed;
	int m_playerId, m_location;
	bool m_isSelf, m_hasTurn;
	QString m_name, m_money;
};

#endif
