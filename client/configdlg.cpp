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

#include "configdlg.h"

#include <tokentheme.h>

#include <settings.h>

ConfigDialog::ConfigDialog(const TokenTheme &theme, QWidget *parent)
	: KConfigDialog(parent, "configdialog", Settings::self())
{
	setWindowTitle(i18n("Configure Atlantik"));
    setFaceType(KPageDialog::List);

	QWidget *w = new QWidget(this);
	m_uiGeneral.setupUi(w);
	w->layout()->setMargin(0);
	w->setObjectName("configGeneral");
	addPage(w, i18n("General"), "configure", i18n("General"));

	w = new QWidget(this);
	m_uiPlayer.setupUi(w);
	m_uiPlayer.kcfg_PlayerImage->setTokenTheme(theme);
	w->layout()->setMargin(0);
	w->setObjectName("configPlayer");
	addPage(w, i18n("Personalization"), "user-identity", i18n("Personalization"));
	
	w = new QWidget(this);
	m_uiBoard.setupUi(w);
	w->layout()->setMargin(0);
	w->setObjectName("configBoard");
	addPage(w, i18n("Board"), "monop_board", i18n("Board"));
	
	w = new QWidget(this);
	m_uiMetaserver.setupUi(w);
	w->layout()->setMargin(0);
	w->setObjectName("configMetaserver");
	addPage(w, i18n("Meta Server"), "network-wired", i18n("Meta Server"));

	setMinimumSize(sizeHint());
}
