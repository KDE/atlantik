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

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocalizedstring.h>
#include <kapplication.h>
#include <kglobal.h>

#include "version.h"
#include "atlantik.h"
#include "monopd.h"

int main(int argc, char *argv[])
{
	KAboutData aboutData(
		"atlantik", 0,
		ki18n("Atlantik"), ATLANTIK_VERSION_STRING,
		ki18n("The Atlantic board game"),
		KAboutData::License_GPL,
		ki18n("(c) 1998-2004 Rob Kaper"),
		ki18n("KDE client for playing Monopoly-like games on the monopd network."),
		"http://unixcode.org/atlantik/"
		);

	aboutData.addAuthor(ki18n("Rob Kaper"), ki18n("main author"), "cap@capsi.com", "http://capsi.com/");

	// Patches and artists
	aboutData.addCredit(ki18n("Thiago Macieira"), ki18n("KExtendedSocket support"), "thiagom@wanadoo.fr");
	aboutData.addCredit(ki18n("Albert Astals Cid"), ki18n("various patches"), "tsdgeos@terra.es");

	aboutData.addCredit(ki18n("Bart Szyszka"), ki18n("application icon"), "bart@gigabee.com", "http://www.gigabee.com/");
	aboutData.addCredit(ki18n("Rob Malda"), ki18n("token icons"), "", "http://cmdrtaco.net/");
	aboutData.addCredit(ki18n("Elhay Achiam"), ki18n("icons"), "elhay_a@bezeqint.net");
	aboutData.addCredit(ki18n("Carlo Caneva"), ki18n("icons"), "webmaster@molecola.com", "http://www.molecola.com/");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KCmdLineOptions options;
	options.add("h");
	options.add("host <argument>", ki18n("Connect to this host"));
	options.add("p");
	options.add("port <argument>", ki18n("Connect at this port"), QByteArray::number(MONOPD_PORT));
	options.add("g");
	options.add("game <argument>", ki18n("Join this game"));
	KCmdLineArgs::addCmdLineOptions (options);

	KCmdLineArgs::addStdCmdLineOptions();
	KApplication kapplication;
    KGlobal::locale()->insertCatalog("libkdegames");

	if (kapplication.isSessionRestored())
		RESTORE(Atlantik)
	else
	{
		Atlantik *atlantik = new Atlantik;
		atlantik->setMinimumSize(640, 480);
		atlantik->setCaption(i18n("The Atlantic Board Game"));
		atlantik->show();
	}

	return kapplication.exec();
}
