// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef ATLANTIK_DISPLAY_WIDGET_H
#define ATLANTIK_DISPLAY_WIDGET_H

#include <qwidget.h>
#include <qlayout.h>
#include <qmap.h>
#include <qvgroupbox.h>
#include <qtextedit.h>

class KPushButton;

class BoardDisplay : public QWidget
{
Q_OBJECT

public:
	BoardDisplay(const QString caption, const QString body, QWidget *parent, const char *name=0);

	void addButton(const QString command, const QString caption, bool enabled);

private slots:
	void buttonPressed();
	void slotClicked();

signals:
	void buttonCommand(QString);

private:
	QTextEdit *m_label;
	QVBoxLayout *m_mainLayout;
	QHBoxLayout *m_buttonBox;
	QVGroupBox *m_textGroupBox;
	QMap <QObject *, QString> m_buttonCommandMap;
};

#endif
