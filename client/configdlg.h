// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
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

#ifndef ATLANTIK_CONFIGDLG_H
#define ATLANTIK_CONFIGDLG_H

#include <kconfigdialog.h>

#include <ui_configboard.h>
#include <ui_configgeneral.h>
#include <ui_configmetaserver.h>
#include <ui_configplayer.h>

class TokenTheme;

class ConfigDialog : public KConfigDialog
{
Q_OBJECT

public:
	ConfigDialog(const TokenTheme &theme, QWidget *parent = nullptr);

private:
	Ui::ConfigGeneral m_uiGeneral;
	Ui::ConfigPlayer m_uiPlayer;
	Ui::ConfigBoard m_uiBoard;
	Ui::ConfigMetaserver m_uiMetaserver;
};

#endif
