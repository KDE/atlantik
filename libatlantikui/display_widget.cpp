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

#include <kdialog.h>
#include <klocale.h>
#include <kpushbutton.h>

#include "display_widget.moc"

BoardDisplay::BoardDisplay(const QString &caption, const QString &body, QWidget *parent, const char *name) : QWidget(parent, name)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());

	m_textGroupBox = new QVGroupBox(caption, this, "groupBox");
	mainLayout->addWidget(m_textGroupBox);

	QTextEdit *label = new QTextEdit(body, NULL, m_textGroupBox);
	label->setReadOnly(true);

	m_buttonBox = new QHBoxLayout(this, 0, KDialog::spacingHint());
	mainLayout->addItem(m_buttonBox);

	m_buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

void BoardDisplay::addButton(const QString &command, const QString &caption, bool enabled)
{
	KPushButton *button = new KPushButton(caption, this);
	m_buttonCommandMap[button] = command;
	m_buttonBox->addWidget(button);

	button->setEnabled(enabled);
	button->show();

	connect(button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
}

void BoardDisplay::addCloseButton()
{
	KPushButton *button = new KPushButton(i18n("Close"), this);
	m_buttonBox->addWidget(button);
	button->show();

	connect(button, SIGNAL(pressed()), this, SIGNAL(buttonClose()));
}

void BoardDisplay::buttonPressed()
{
	QObject *o = const_cast<QObject*>(QObject::sender());
	emit buttonCommand(m_buttonCommandMap[o]);
}

void BoardDisplay::slotClicked()
{
	m_textGroupBox->setEnabled(true);
}
