#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapplication.h>

#include "main.h"
#include "config.h"
#include "atlantik.h"
#include "colordefs.h"

AtlantikConfig atlantikConfig;

int main(int argc, char *argv[])
{
	KAboutData aboutData(
		"atlantik",
		I18N_NOOP("Atlantik"), ATLANTIK_VERSION_STRING,
		I18N_NOOP("The Atlantic board game"),
		KAboutData::License_GPL,
		I18N_NOOP("(c) 2001 Rob Kaper"),
		I18N_NOOP("KDE client for playing games out of the collection of the Atlantic board game daemon."),
		"http://capsi.com/atlantik/"
		);

	aboutData.addAuthor("Rob Kaper", I18N_NOOP("main author"), "cap@capsi.com", "http://capsi.com/");
	aboutData.addAuthor("Jason Katz-Brown", I18N_NOOP("gameboard designer"), "jason@katzbrown.com", "http://katzbrown.com/");

	aboutData.addCredit("Bart Szyszka", I18N_NOOP("application icon"), "bart@gigabee.com", "http://www.gigabee.com/");
	aboutData.addCredit("Carlo Caneva", I18N_NOOP("various icons"), "webmaster@molecola.com", "http://www.molecola.com/");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication kapplication;

	if (kapplication.isRestored())
		RESTORE(Atlantik)
	else
	{
		Atlantik *atlantik = new Atlantik;
		atlantik->setMinimumSize(640, 480);
		atlantik->setCaption(i18n("The Atlantic board game"));
		atlantik->show();
	}

	return kapplication.exec();
}