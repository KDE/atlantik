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
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qtextedit.h>

class BoardDisplay : public QWidget
{
Q_OBJECT

public:
	BoardDisplay(const QString type, const QString description, QWidget *parent, const char *name=0);

private slots:
	void slotClicked();

private:
	QTextEdit *m_label;
	QVBoxLayout *m_mainLayout;
	QVGroupBox *m_playerGroupBox;
};

#endif
