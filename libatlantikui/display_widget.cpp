// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License version 2 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <kdebug.h>

#include <kdialog.h>
#include <klocale.h>
 
#include "display_widget.moc"

BoardDisplay::BoardDisplay(const QString type, const QString description, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	Q_CHECK_PTR(m_mainLayout);

	// Player list.
	m_playerGroupBox = new QVGroupBox(type, this, "groupBox");
	m_mainLayout->addWidget(m_playerGroupBox); 

	m_label = new QTextEdit(description, NULL, m_playerGroupBox);
	m_label->setReadOnly(true);
}

void BoardDisplay::slotClicked()
{
	m_playerGroupBox->setEnabled(true);
}
