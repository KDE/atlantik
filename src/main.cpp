#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapp.h>

#include "main.h"
#include "config.h"
#include "kmonop.h"
#include "colordefs.h"

KMonopConfig kmonopConfig;

int main(int argc, char *argv[])
{
	KAboutData aboutData(
		"kmonop",
		I18N_NOOP("KMonop"), VERSION,
		I18N_NOOP("Monopoly boardgame"),
		KAboutData::License_GPL,
		I18N_NOOP("(c) 2001 Rob Kaper"),
		I18N_NOOP("KDE client for playing the Monopoly boardgame on monopd servers."),
		"http://capsi.com/kmonop/"
		);

	aboutData.addAuthor("Rob Kaper", I18N_NOOP("programming"), "cap@capsi.com", "http://capsi.com/");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication kapplication;

	if (kapplication.isRestored())
		RESTORE(KMonop)
	else
	{
		KMonop *kmonop = new KMonop;
		kmonop->setGeometry(0, 0, 640, 480);
		kmonop->setCaption("Monopoly for KDE");
		kmonop->show();
	}

	return kapplication.exec();
}
