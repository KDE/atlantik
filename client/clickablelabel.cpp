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

#include "clickablelabel.h"

#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget *parent)
	: QLabel(parent)
{
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		Q_EMIT clicked();
}

#include "moc_clickablelabel.cpp"
