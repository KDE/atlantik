#include <qapplication.h>

#include "atlanticdaemon.h"

int main(int argc, char *argv[])
{
	AtlanticDaemon *atlanticDaemon = new AtlanticDaemon();

	QApplication qapplication(argc, argv);
	qapplication.exec();
}
