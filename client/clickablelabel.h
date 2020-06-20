// Copyright (c) 2017 Pino Toscano <pino@kde.org>
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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_CLICKABLELABEL_H
#define ATLANTIK_CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
Q_OBJECT

public:
	ClickableLabel(QWidget *parent = nullptr);
	~ClickableLabel();

Q_SIGNALS:
	void clicked();

protected:
	void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif
