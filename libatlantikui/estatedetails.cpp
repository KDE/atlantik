// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
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

#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include <QFontDatabase>

#include <kiconloader.h>
#include <klocalizedstring.h>
#include <KStandardGuiItem>

#include <estate.h>
#include <player.h>

#include "estatedetails.h"

static QIcon iconForCommandButton(const QString &command)
{
	if (command == QLatin1String(".r"))
		return KDE::icon(QStringLiteral("roll"));
	if (command == QLatin1String(".eb"))
		return KDE::icon(QStringLiteral("atlantik_buy_estate"));
	if (command == QLatin1String(".ea"))
		return KDE::icon(QStringLiteral("auction"));
	if (command == QLatin1String(".E"))
		return KDE::icon(QStringLiteral("games-endturn"));
	if (command == QLatin1String(".jp"))
		return KDE::icon(QStringLiteral("jail_pay"));
	return QIcon();
}

EstateDetails::EstateDetails(Estate *estate, const QString &text, QWidget *parent)
	: EstateDetailsBase(estate, parent)
	, m_closeButton(nullptr)
{
	m_buttons.reserve(3); // Usually there are no more than 3 action buttons

	QWidget *w = widget();

	QVBoxLayout *mainLayout = new QVBoxLayout(w);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	m_infoListView = new QListWidget(w);
	m_infoListView->setWordWrap(true);
	mainLayout->addWidget(m_infoListView);

	appendText(text);

	m_buttonBox = new QHBoxLayout();
	mainLayout->addItem(m_buttonBox);

	m_buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

EstateDetails::~EstateDetails()
{
	delete m_infoListView;
}

void EstateDetails::addDetails()
{
	Estate *e = estate();

	if (e)
	{
		QListWidgetItem *infoText = nullptr;
		const QIcon infoIcon = KDE::icon(QStringLiteral("document-properties"));

		// Price
		if (e->price())
		{
			infoText = new QListWidgetItem();
			infoText->setText(i18n("Price: %1", e->price()));
			infoText->setIcon(infoIcon);
			m_infoListView->addItem(infoText);
		}

		// Owner, houses, isMortgaged
		if (e->canBeOwned())
		{
			infoText = new QListWidgetItem();
			infoText->setText(i18n("Owner: %1", e->owner() ? e->owner()->name() : i18n("unowned")));
			infoText->setIcon(infoIcon);
			m_infoListView->addItem(infoText);

			if (e->isOwned())
			{
				infoText = new QListWidgetItem();
				infoText->setText(i18n("Houses: %1", e->houses()));
				infoText->setIcon(infoIcon);
				m_infoListView->addItem(infoText);

				infoText = new QListWidgetItem();
				infoText->setText(i18n("Mortgaged: %1", e->isMortgaged() ? i18n("Yes") : i18n("No")));
				infoText->setIcon(infoIcon);
				m_infoListView->addItem(infoText);
			}
		}
	}
}

void EstateDetails::addButton(const QString &command, const QString &caption, bool enabled)
{
	QPushButton *button = new QPushButton(iconForCommandButton(command), caption, widget());
	m_buttons.append(button);
	m_buttonBox->addWidget(button);

	Estate *e = estate();

	if (e)
	{
		QColor bgColor, fgColor;
		bgColor = e->bgColor().lighter(110);
		fgColor = ( bgColor.red() + bgColor.green() + bgColor.blue() < 255 ) ? Qt::white : Qt::black;

		QPalette pal = button->palette();
		pal.setColor( button->foregroundRole(), fgColor );
		pal.setColor( button->backgroundRole(), bgColor );
		button->setPalette( pal );
	}
	button->setEnabled(enabled);
	button->show();

	connect(button, &QPushButton::pressed, this, [this, command]() { buttonCommand(command); });
}

void EstateDetails::addCloseButton()
{
	if (!m_closeButton)
	{
		m_closeButton = new QPushButton(widget());
		KStandardGuiItem::assign(m_closeButton, KStandardGuiItem::Close);
		m_buttonBox->addWidget(m_closeButton);
		m_closeButton->show();
		connect(m_closeButton, SIGNAL(pressed()), this, SIGNAL(buttonClose()));
	}
}

void EstateDetails::setText(const QString &text)
{
	m_infoListView->clear();
	appendText(text);
}

void EstateDetails::appendText(const QString &text)
{
	if ( text.isEmpty() )
		return;

	QListWidgetItem *infoText = new QListWidgetItem();
	infoText->setText(text);
	if (text.contains(QLatin1String(" rolls ")))
		infoText->setIcon(KDE::icon(QStringLiteral("roll")));
	else
		infoText->setIcon(KDE::icon(QStringLiteral("atlantik")));
	m_infoListView->addItem(infoText);

	m_infoListView->scrollToItem(infoText);
}

void EstateDetails::clearButtons()
{
	if (m_closeButton)
	{
		delete m_closeButton;
		m_closeButton = nullptr;
	}

	// Delete buttons
	qDeleteAll(m_buttons);
	m_buttons.resize(0); // Keep the capacity allocated
}

#include "moc_estatedetails.cpp"
