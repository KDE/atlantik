#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapp.h>

#include "main.h"
#include "config.h"
#include "atlantik.h"
#include "colordefs.h"

AtlantikConfig atlantikConfig;

int main(int argc, char *argv[])
{
	KAboutData aboutData(
		"atlantik",
		I18N_NOOP("Atlantik"), VERSION,
		I18N_NOOP("The Atlantic board game"),
		KAboutData::License_GPL,
		I18N_NOOP("(c) 2001 Rob Kaper"),
		I18N_NOOP("KDE client for playing games out of the collection of the Atlantic board game daemon."),
		"http://capsi.com/atlantik/"
		);

	aboutData.addAuthor("Rob Kaper", I18N_NOOP("programming"), "cap@capsi.com", "http://capsi.com/");
	aboutData.addAuthor("Bart Szyszka", I18N_NOOP("artwork"), "bart@gigabee.com", "http://www.gigabee.com/");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication kapplication;

	if (kapplication.isRestored())
		RESTORE(Atlantik)
	else
	{
		Atlantik *atlantik = new Atlantik;
		atlantik->setGeometry(0, 0, 640, 480);
		atlantik->setCaption(i18n("The Atlantic board game"));
		atlantik->show();
	}

	return kapplication.exec();
}
