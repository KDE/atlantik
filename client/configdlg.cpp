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

#include <qlayout.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qlabel.h>

#include <kiconloader.h>
#include <klocale.h>

#include "atlantik.h"
#include "configdlg.moc"

ConfigDialog::ConfigDialog(Atlantik* parent, const char *name) : KDialogBase(IconList, i18n("Configure Atlantik"), Ok|Cancel, Ok, parent, "config_atlantik", false, name)
{
	m_parent = parent;
	p_p13n = addPage(i18n("Personalization"), i18n("Personalization"), BarIcon("personal", KIcon::SizeMedium));
	p_board = addPage(i18n("Board"), i18n("Board"), BarIcon("monop_board", KIcon::SizeMedium));

	configPlayer = new ConfigPlayer(this, p_p13n, "configPlayer");
	configBoard = new ConfigBoard(this, p_board, "configBoard");

	setMinimumSize(sizeHint());
}

bool ConfigDialog::indicateUnowned()
{
	return configBoard->indicateUnowned();
}

bool ConfigDialog::highliteUnowned()
{
	return configBoard->highliteUnowned();
}

bool ConfigDialog::darkenMortgaged()
{
	return configBoard->darkenMortgaged();
}

bool ConfigDialog::animateToken()
{
	return configBoard->animateToken();
}

bool ConfigDialog::quartzEffects()
{
	return configBoard->quartzEffects();
}

QString ConfigDialog::playerName()
{
	return configPlayer->playerName();
}

AtlantikConfig ConfigDialog::config()
{
	return m_parent->config();
}

ConfigPlayer::ConfigPlayer(ConfigDialog* configDialog, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QLabel *label = new QLabel(i18n("Player name:"), parent);
	layout->addWidget(label);

	m_playerName = new QLineEdit(parent);
	layout->addWidget(m_playerName);

	layout->addStretch(1);

	reset();
}

QString ConfigPlayer::playerName()
{
	return m_playerName->text();
}

void ConfigPlayer::reset()
{
	m_playerName->setText(m_configDialog->config().playerName);
}

ConfigBoard::ConfigBoard(ConfigDialog *configDialog, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_configDialog = configDialog;
	QVBoxLayout *layout = new QVBoxLayout(parent, KDialog::marginHint(), KDialog::spacingHint());

	QGroupBox *box = new QGroupBox(1, Qt::Horizontal, i18n("Game Status Feedback"), parent);
	layout->addWidget(box);

	m_indicateUnowned = new QCheckBox(i18n("Display title deed card on unowned properties"), box);
	QString message=i18n(
		"If checked, unowned properties on the board display an estate\n"
		"card to indicate the property is for sale.\n");
	QWhatsThis::add(m_indicateUnowned, message);

	m_highliteUnowned = new QCheckBox(i18n("Highlight unowned properties"), box);
	message=i18n(
		"If checked, unowned properties on the board are highlighted to\n"
		"indicate the property is for sale.\n");
	QWhatsThis::add(m_highliteUnowned, message);

	m_darkenMortgaged = new QCheckBox(i18n("Darken mortgaged properties"), box);
	message=i18n(
		"If checked, mortgaged properties on the board will be colored\n"
		"darker than of the default color.\n");
	QWhatsThis::add(m_darkenMortgaged, message);

	m_animateToken = new QCheckBox(i18n("Animate token movement"), box);
	message=i18n(
		"If checked, tokens will move across the board\n"
		"instead of jumping directly to their new location.\n");
	QWhatsThis::add(m_animateToken, message);

	m_quartzEffects = new QCheckBox(i18n("Quartz effects"), box);
	message=i18n(
		"If checked, the colored headers of street estates on the board "
		"will have a Quartz effect similar to the Quartz KWin style.\n");
	QWhatsThis::add(m_quartzEffects, message);

//	box = new QGroupBox(1, Qt::Horizontal, i18n("Size"), parent);
//	layout->addWidget(box);

	layout->addStretch(1);

	reset();
}

bool ConfigBoard::indicateUnowned()
{
	return m_indicateUnowned->isChecked();
}

bool ConfigBoard::highliteUnowned()
{
	return m_highliteUnowned->isChecked();
}

bool ConfigBoard::darkenMortgaged()
{
	return m_darkenMortgaged->isChecked();
}

bool ConfigBoard::animateToken()
{
	return m_animateToken->isChecked();
}

bool ConfigBoard::quartzEffects()
{
	return m_quartzEffects->isChecked();
}

void ConfigBoard::reset()
{
	m_indicateUnowned->setChecked(m_configDialog->config().indicateUnowned);
	m_highliteUnowned->setChecked(m_configDialog->config().highliteUnowned);
	m_darkenMortgaged->setChecked(m_configDialog->config().darkenMortgaged);
	m_animateToken->setChecked(m_configDialog->config().animateTokens);
	m_quartzEffects->setChecked(m_configDialog->config().quartzEffects);
}
