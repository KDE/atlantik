#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapp.h>

#include <qwidget.h>
#include <qpainter.h>
// #include <iostream.h>

#include "colordefs.h"
#include "kmonop.h"

int main(int, char *[]);

int main(int argc, char *argv[])
{
	KAboutData aboutData(
		"kmonop",
		I18N_NOOP("KMonop"), "0.0.1",
		I18N_NOOP("Monopoly boardgame"),
		KAboutData::License_GPL,
		I18N_NOOP("(c) 2001 Rob Kaper"),
		I18N_NOOP("KDE client for playing the Monopoly boardgame on monopd servers."),
		"http://capsi.com/kmonop/"
		);

	aboutData.addAuthor("Rob Kaper", I18N_NOOP("programming"), "cap@capsi.com", "http://capsi.com/");
	aboutData.addAuthor("Martin de Gast", I18N_NOOP("graphics"), "martin@kira-nerys.com", "http://kira-nerys.com/");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication kapplication;

//	KApplication kapplication(argc, argv, "kmonop");
	
	if (kapplication.isRestored())
		RESTORE(KMonop)
	else
	{
		KMonop *kmonop = new KMonop;
		kmonop->setGeometry(0, 0, 640, 480);
		kmonop->setCaption("Monopoly for KDE");
		kmonop->show();
	}

//	kapplication.setMainWidget(&window);

	return kapplication.exec();
}
