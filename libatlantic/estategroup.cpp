#include "estategroup.h"
#include "estategroup.moc"

EstateGroup::EstateGroup(const QString name) : QObject()
{
	m_name = name;
}

void EstateGroup::setName(const QString name)
{
	if (m_name != name)
	{
		m_name = name;
		m_changed = true;
	}
}

void EstateGroup::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}
