#ifndef LIBATLANTIC_PLAYER_H
#define LIBATLANTIC_PLAYER_H

#include <qobject.h>
#include <qstring.h>

class Estate;

class Player : public QObject
{
Q_OBJECT

public:
	Player(int playerId);
	int playerId() { return m_playerId; }

	void setLocation(Estate *estate);
	Estate *location() { return m_location; }
	void setIsSelf(const bool isSelf) { m_isSelf = isSelf; }
	bool isSelf() const { return m_isSelf; }
	void setHasTurn(const bool hasTurn);
	bool hasTurn() const { return m_hasTurn; }
	void setInJail(const bool inJail);
	bool inJail() const { return m_inJail; }
	void setName(const QString _n);
	QString name() const { return m_name; }
	void setMoney(unsigned int _m);
	unsigned int money() const { return m_money; }
	void update(bool force = false);

signals:
	void changed();

private:
	bool m_changed, m_isSelf, m_hasTurn, m_inJail;
	int m_playerId;
	unsigned int m_money;
	QString m_name;
	Estate *m_location;
};

#endif
