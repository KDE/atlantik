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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_ESTATEDETAILS_H
#define ATLANTIK_ESTATEDETAILS_H

#include "estatedetailsbase.h"

#include <QVector>

class QHBoxLayout;

class QListWidget;
class QPushButton;

class EstateDetails : public EstateDetailsBase
{
Q_OBJECT

public:
	EstateDetails(Estate *estate, const QString &text, QWidget *parent);
	~EstateDetails();

	void addDetails();
	void addButton(const QString &command, const QString &caption, bool enabled);
	void addCloseButton();
	void setText(const QString &text);
	void appendText(const QString &text);
	void clearButtons();

Q_SIGNALS:
	void buttonCommand(const QString&);
	void buttonClose();

private:
	QListWidget *m_infoListView;
	QPushButton *m_closeButton;
	QHBoxLayout *m_buttonBox;
	QVector<QPushButton *> m_buttons;
};

#endif
