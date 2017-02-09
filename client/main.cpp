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

#include <QApplication>
#include <QCommandLineParser>

#include <kaboutdata.h>
#include <klocalizedstring.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "version.h"
#include "atlantik.h"
#include "monopd.h"

int main(int argc, char *argv[])
{
	QApplication kapplication(argc, argv);
	KLocalizedString::setApplicationDomain("atlantik");

	KAboutData aboutData(
		"atlantik",
		i18n("Atlantik"), ATLANTIK_VERSION_STRING,
		i18n("The Atlantic board game"),
		KAboutLicense::GPL,
		i18n("(c) 1998-2004 Rob Kaper"),
		i18n("KDE client for playing Monopoly-like games on the monopd network."),
		"http://unixcode.org/atlantik/"
		);

	aboutData.addAuthor(i18n("Rob Kaper"), i18n("main author"), "cap@capsi.com", "http://capsi.com/");

	// Patches and artists
	aboutData.addCredit(i18n("Thiago Macieira"), i18n("KExtendedSocket support"), "thiagom@wanadoo.fr");
	aboutData.addCredit(i18n("Albert Astals Cid"), i18n("various patches"), "tsdgeos@terra.es");

	aboutData.addCredit(i18n("Bart Szyszka"), i18n("application icon"), "bart@gigabee.com", "http://www.gigabee.com/");
	aboutData.addCredit(i18n("Rob Malda"), i18n("token icons"), "", "http://cmdrtaco.net/");
	aboutData.addCredit(i18n("Elhay Achiam"), i18n("icons"), "elhay_a@bezeqint.net");
	aboutData.addCredit(i18n("Carlo Caneva"), i18n("icons"), "webmaster@molecola.com", "http://www.molecola.com/");

	KAboutData::setApplicationData(aboutData);

	kapplication.setWindowIcon(KDE::icon("atlantik"));

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addOption(QCommandLineOption(QStringList() << "host", i18n("Connect to this host"), i18n("host")));
	parser.addOption(QCommandLineOption(QStringList() << "p" << "port", i18n("Connect at this port"), i18n("port"), QString::number(MONOPD_PORT)));
	parser.addOption(QCommandLineOption(QStringList() << "g" << "game", i18n("Join this game"), i18n("game")));
	aboutData.setupCommandLine(&parser);
	parser.process(kapplication);
	aboutData.processCommandLine(&parser);
    KGlobal::locale()->insertCatalog("libkdegames");

	if (kapplication.isSessionRestored())
		kRestoreMainWindows<Atlantik>();
	else
	{
		Atlantik *atlantik = new Atlantik(&parser);
		atlantik->setMinimumSize(640, 480);
		atlantik->setCaption(i18n("The Atlantic Board Game"));
		atlantik->show();
	}

	return kapplication.exec();
}
