#ifndef LIBATLANTIC_ESTATEGROUP_H
#define LIBATLANTIC_ESTATEGROUP_H

#include <qobject.h>

class EstateGroup : public QObject
{
Q_OBJECT

public:
	EstateGroup(const QString name);
	void setName(const QString name);
	QString name() const { return m_name; }
	void update(bool force = false);

signals:
	void changed();

private:
	bool m_changed;
	QString m_name;
};

#endif
