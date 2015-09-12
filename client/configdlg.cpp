// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
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

#include <QLayout>
#include <qgroupbox.h>

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

#include <kdeversion.h>
#include <klocale.h>
#include <kicon.h>

#include "atlantik.h"
#include "configdlg.moc"

ConfigDialog::ConfigDialog(Atlantik* parent) : 
		KPageDialog(parent)
{
    setCaption(i18n("Configure Atlantik"));
    setButtons(Ok|Cancel);
    setDefaultButton(Ok);
    setModal(false);
    setFaceType(KPageDialog::List);
    m_parent = parent;
	configGeneral = new ConfigGeneral(this, 0);
	configGeneral->setObjectName("configGeneral");
	KPageWidgetItem *pageItem = new KPageWidgetItem(configGeneral, i18n("General"));
    pageItem->setHeader(i18n("General"));
	pageItem->setIcon(KIcon("configure"));
    addPage(pageItem);

	configPlayer = new ConfigPlayer(this, 0);
	configPlayer->setObjectName("configPlayer");
	pageItem = new KPageWidgetItem(configPlayer, i18n("Personalization"));
    pageItem->setHeader(i18n("Personalization"));
	pageItem->setIcon(KIcon("user-identity"));
    addPage(pageItem);	
	
	configBoard = new ConfigBoard(this, 0);
	configBoard->setObjectName("configBoard" );
    pageItem = new KPageWidgetItem(configBoard, i18n("Board"));
    pageItem->setHeader(i18n("Board"));
	pageItem->setIcon(KIcon("monop_board"));
    addPage(pageItem);
	
	configMonopigator = new ConfigMonopigator(this, 0);
	configMonopigator->setObjectName("configMonopigator");
	pageItem = new KPageWidgetItem(configMonopigator, i18n("Meta Server"));
    pageItem->setHeader(i18n("Meta Server"));
	pageItem->setIcon(KIcon("network-wired"));
    addPage(pageItem);

	setMinimumSize(sizeHint());
}

bool ConfigDialog::chatTimestamps()
{
	return configGeneral->chatTimestamps();
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

QString ConfigDialog::playerImage()
{
	return configPlayer->playerImage();
}

bool ConfigDialog::connectOnStart()
{
	return configMonopigator->connectOnStart();
}

bool ConfigDialog::hideDevelopmentServers()
{
	return configMonopigator->hideDevelopmentServers();
}

AtlantikConfig ConfigDialog::config()
{
	return m_parent->config();
}

ConfigPlayer::ConfigPlayer(ConfigDialog* configDialog, QWidget *parent) : QWidget(parent)
{
	m_configDialog = configDialog;
	m_ui.setupUi(this);
	layout()->setMargin(0);

	m_ui.kcfg_PlayerImage->setLocation("appdata", "themes/default/tokens/");

	reset();
}

QString ConfigPlayer::playerName()
{
	return m_ui.kcfg_PlayerName->text();
}

QString ConfigPlayer::playerImage()
{
	return m_ui.kcfg_PlayerImage->image();
}

void ConfigPlayer::reset()
{
	m_ui.kcfg_PlayerName->setText(m_configDialog->config().playerName);
	m_ui.kcfg_PlayerImage->setImage(m_configDialog->config().playerImage);
}

ConfigMonopigator::ConfigMonopigator(ConfigDialog *configDialog, QWidget *parent) : QWidget(parent)
{
	m_configDialog = configDialog;
	m_ui.setupUi(this);
	layout()->setMargin(0);

	reset();
}

bool ConfigMonopigator::connectOnStart()
{
	return m_ui.kcfg_ConnectOnStart->isChecked();
}

bool ConfigMonopigator::hideDevelopmentServers()
{
	return m_ui.kcfg_HideDevelopmentServers->isChecked();
}

void ConfigMonopigator::reset()
{
	m_ui.kcfg_ConnectOnStart->setChecked(m_configDialog->config().connectOnStart);
	m_ui.kcfg_HideDevelopmentServers->setChecked(m_configDialog->config().hideDevelopmentServers);
}

ConfigGeneral::ConfigGeneral(ConfigDialog *configDialog, QWidget *parent) : QWidget(parent)
{
	m_configDialog = configDialog;
	m_ui.setupUi(this);
	layout()->setMargin(0);

	reset();
}

bool ConfigGeneral::chatTimestamps()
{
	return m_ui.kcfg_ChatTimeStamps->isChecked();
}

void ConfigGeneral::reset()
{
	m_ui.kcfg_ChatTimeStamps->setChecked(m_configDialog->config().chatTimestamps);
}

ConfigBoard::ConfigBoard(ConfigDialog *configDialog, QWidget *parent) : QWidget(parent)
{
	m_configDialog = configDialog;
	m_ui.setupUi(this);
	layout()->setMargin(0);

	reset();
}

bool ConfigBoard::indicateUnowned()
{
	return m_ui.kcfg_IndicateUnowned->isChecked();
}

bool ConfigBoard::highliteUnowned()
{
	return m_ui.kcfg_HighliteUnowned->isChecked();
}

bool ConfigBoard::darkenMortgaged()
{
	return m_ui.kcfg_DarkenMortgaged->isChecked();
}

bool ConfigBoard::animateToken()
{
	return m_ui.kcfg_AnimateToken->isChecked();
}

bool ConfigBoard::quartzEffects()
{
	return m_ui.kcfg_QuartzEffects->isChecked();
}

void ConfigBoard::reset()
{
	m_ui.kcfg_IndicateUnowned->setChecked(m_configDialog->config().indicateUnowned);
	m_ui.kcfg_HighliteUnowned->setChecked(m_configDialog->config().highliteUnowned);
	m_ui.kcfg_DarkenMortgaged->setChecked(m_configDialog->config().darkenMortgaged);
	m_ui.kcfg_AnimateToken->setChecked(m_configDialog->config().animateTokens);
	m_ui.kcfg_QuartzEffects->setChecked(m_configDialog->config().quartzEffects);
}
