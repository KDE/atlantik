// Copyright (c) 2003-2004 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <iostream>

#include <qheader.h>
#include <qlayout.h>
#include <qdatetime.h>

#include <klocale.h>
#include <klistview.h>
#include <kdialogbase.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <kstringhandler.h>

#include "event.h"
#include "eventlogwidget.moc"

EventLog::EventLog()
{
}

void EventLog::addEvent(const QString &description, const QString &icon)
{
	Event *event = new Event(QDateTime::currentDateTime(), description, icon);
	m_events.append(event);
	emit newEvent(event);
}

QPtrList<Event> EventLog::events()
{
	return m_events;
}

EventLogWidget::EventLogWidget(EventLog *eventLog, QWidget *parent, const char *name)
	: QWidget(parent, name,
	  WType_Dialog | WStyle_Customize | WStyle_DialogBorder | WStyle_Title |
	  WStyle_Minimize | WStyle_ContextHelp )
{
	m_eventLog = eventLog;

	connect(m_eventLog, SIGNAL(newEvent(Event *)), this, SLOT(addEvent(Event *)));

	setCaption(i18n("Event Log"));

	QVBoxLayout *listCompBox = new QVBoxLayout(this, KDialog::marginHint());

	m_eventList = new KListView(this, "eventList");
	listCompBox->addWidget(m_eventList);

	m_eventList->addColumn(i18n("Date/Time"));
	m_eventList->addColumn(i18n("Description"));
	m_eventList->header()->setClickEnabled( false );

	QHBoxLayout *actionBox = new QHBoxLayout(this, 0, KDialog::spacingHint());
	listCompBox->addItem(actionBox);

	actionBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_saveButton = new KPushButton(BarIcon("filesave", KIcon::SizeSmall), i18n("&Save As..."), this);
	actionBox->addWidget(m_saveButton);

	connect(m_saveButton, SIGNAL(clicked()), this, SLOT(save()));

	// Populate
	QPtrList<Event> events = m_eventLog->events();
	for (QPtrListIterator<Event> it( events ); (*it) ; ++it)
		addEvent( (*it) );
}

void EventLogWidget::addEvent(Event *event)
{
	// FIXME: allow a way to view non-squeezed message
	// FIXME: allow a way to show older messages

	if ( m_eventList->childCount() >= 25 )
		delete m_eventList->firstChild();

	QString description = KStringHandler::rsqueeze( event->description(), 200 );
	KListViewItem *item = new KListViewItem(m_eventList, event->dateTime().toString("yyyy-MM-dd hh:mm:ss zzz"), description);
	if (event->icon().isEmpty())
		item->setPixmap(1, QPixmap(SmallIcon("atlantik")));
	else
		item->setPixmap(1, QPixmap(SmallIcon(event->icon())));

	m_eventList->ensureItemVisible(item);
}

void EventLogWidget::closeEvent(QCloseEvent *e)
{
	e->accept();
}

void EventLogWidget::save()
{
	QFile file( KFileDialog::getSaveFileName() );
	if ( file.open( IO_WriteOnly ) )
	{
		QTextStream stream(&file);

		stream << i18n( "Atlantik log file, saved at %1." ).arg( QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") ) << endl;

		QPtrList<Event> events = m_eventLog->events();
		for (QPtrListIterator<Event> it( events ); (*it) ; ++it)
			stream << (*it)->dateTime().toString("yyyy-MM-dd hh:mm:ss") << " " << (*it)->description() << endl;
		file.close();
	}
}
