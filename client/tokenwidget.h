// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef ATLANTIK_TOKENWIDGET_H
#define ATLANTIK_TOKENWIDGET_H

#include <qwidget.h>

class QVBoxLayout;

class KFileItem;

class TokenWidget : public QWidget
{
Q_OBJECT

public:
	TokenWidget(QWidget *parent, const char *name=0);

public slots:
	void slotIconSelected(const KFileItem *item);

signals:
	void iconSelected(const QString &name);

private:
	QVBoxLayout *m_mainLayout;
};

#endif