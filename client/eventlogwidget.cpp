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

#include <q3header.h>
#include <QLayout>
#include <QDateTime>
//Added by qt3to4:
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextStream>
#include <QCloseEvent>

#include <klocale.h>
#include <k3listview.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <kstringhandler.h>

#include "event.h"
#include "eventlogwidget.moc"

EventLog::EventLog()
	: QObject()
{
}

EventLog::~EventLog()
{
	qDeleteAll(m_events);
}

void EventLog::addEvent(const QString &description, const QString &icon)
{
	Event *event = new Event(QDateTime::currentDateTime(), description, icon);
	m_events.append(event);
	emit newEvent(event);
}

QList<Event*> EventLog::events()
{
	return m_events;
}

EventLogWidget::EventLogWidget(EventLog *eventLog, QWidget *parent)
	: KDialog(parent,
	  Qt::WindowContextHelpButtonHint)
{
	setButtons(Close|User1);
	setDefaultButton(Close);
	setButtonGuiItem(User1, KGuiItem(i18n("&Save As..."), "document-save"));
	setModal(false);

	m_eventLog = eventLog;

	connect(m_eventLog, SIGNAL(newEvent(Event *)), this, SLOT(addEvent(Event *)));

	setWindowTitle(i18n("Event Log"));

	QVBoxLayout *listCompBox = new QVBoxLayout(mainWidget());
	listCompBox->setSpacing(KDialog::marginHint());

	m_eventList = new K3ListView(mainWidget());
	m_eventList->setObjectName( "eventList" );
	listCompBox->addWidget(m_eventList);

	m_eventList->addColumn(i18n("Date/Time"));
	m_eventList->addColumn(i18n("Description"));
	m_eventList->header()->setClickEnabled( false );

	connect(this, SIGNAL(user1Clicked()), this, SLOT(save()));

	// Populate
	foreach (Event *e, m_eventLog->events())
		addEvent(e);
}

void EventLogWidget::addEvent(Event *event)
{
	// FIXME: allow a way to view non-squeezed message
	// FIXME: allow a way to show older messages

	if ( m_eventList->childCount() >= 25 )
		delete m_eventList->firstChild();

	QString description = KStringHandler::rsqueeze( event->description(), 200 );
	K3ListViewItem *item = new K3ListViewItem(m_eventList, event->dateTime().toString("yyyy-MM-dd hh:mm:ss zzz"), description);
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
	if ( file.open( QIODevice::WriteOnly ) )
	{
		QTextStream stream(&file);

		stream << i18n( "Atlantik log file, saved at %1.", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") ) << endl;

		foreach (Event *e, m_eventLog->events())
			stream << e->dateTime().toString("yyyy-MM-dd hh:mm:ss") << " " << e->description() << endl;
		file.close();
	}
}
