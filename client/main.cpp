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

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapplication.h>

#include "main.h"
#include "atlantik.h"

int main(int argc, char *argv[])
{
	KAboutData aboutData(
		"atlantik",
		I18N_NOOP("Atlantik"), ATLANTIK_VERSION_STRING,
		I18N_NOOP("The Atlantic board game"),
		KAboutData::License_GPL,
		I18N_NOOP("(c) 1998-2002 Rob Kaper"),
		I18N_NOOP("KDE client for playing Monopoly-like games on the monopd network."),
		"http://capsi.com/atlantik/"
		);

	aboutData.addAuthor("Rob Kaper", I18N_NOOP("main author"), "cap@capsi.com", "http://capsi.com/");

	// Patches and artists
	aboutData.addCredit("Thiago Macieira", I18N_NOOP("KExtendedSocket support"), "thiagom@wanadoo.fr");

	aboutData.addCredit("Bart Szyszka", I18N_NOOP("application icon"), "bart@gigabee.com", "http://www.gigabee.com/");
	aboutData.addCredit("Elhay Achiam", I18N_NOOP("icons"), "elhay_a@bezeqint.net");
	aboutData.addCredit("Carlo Caneva", I18N_NOOP("icons"), "webmaster@molecola.com", "http://www.molecola.com/");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication kapplication;

	if (kapplication.isRestored())
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
