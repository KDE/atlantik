//jason@katzbrown.com

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapp.h>
#include <klocale.h>
#include <qnamespace.h>
#include <kmainwindow.h>

#include "designer.h"

static KCmdLineOptions options[] =
{
	{ 0, 0, 0 }
	// INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
	KAboutData aboutData( "atlanticdesigner", I18N_NOOP("Atlantic Designer"),
	  "0.5", I18N_NOOP("Gameboard designer for Atlantic"), KAboutData::License_GPL,
	  "(c) 2001, Jason Katz-Brown", 0, "http://www.katzbrown.com/atlanticdesigner", "jason@katzbrown.com");

	aboutData.addAuthor("Jason Katz-Brown", I18N_NOOP("Original author"), "jason@katzbrown.com");
	aboutData.addAuthor("Rob Kaper", I18N_NOOP("Author of Atlantic"), "cap@capsi.com");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

	KApplication a;
	TopLevel *t = new TopLevel();
	t->show();
	return a.exec();
}
