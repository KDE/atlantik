#include <kmainwindow.h>
#include <klocale.h>
#include <kglobal.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <qwidget.h>
#include <kmessagebox.h>
#include <qstring.h>
#include <kapp.h>
#include <kconfig.h>
#include <qevent.h>
#include <qcolor.h>
#include <kstandarddirs.h>
#include <kglobalaccel.h>
#include <kdebug.h>
#include <qlayout.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <kstdaction.h>

#include "designer.h"
#include "editor.h"

TopLevel::TopLevel(QWidget *parent, const char *name) : KMainWindow(parent, name)
{
	max = 40;

	editor = new EstateEdit(this, "Estate Editor");
	setCentralWidget(editor);

	(void) KStdAction::quit(this, SLOT(close()), actionCollection());
	(void) KStdAction::open(this, SLOT(open()), actionCollection());
	(void) KStdAction::openNew(this, SLOT(openNew()), actionCollection());
	(void) KStdAction::save(this, SLOT(save()), actionCollection());

	estateAct = new KListAction(i18n("Estate Number"), 0, 0, 0, actionCollection(), "estate_num");
	QStringList estates;
	for (int i = 1; i <= max; i++)
		estates.append(QString::number(i));
	estateAct->setItems(estates);
	connect(estateAct, SIGNAL(activated(int)), SLOT(changeEstate(int)));

	createGUI();

	filename = QString::null;
	openNew();

	// these MUST match up to the ones in editor.cpp!
	types.append("street");
	types.append("airport");
	types.append("chance");
	types.append("street");
	types.append("go");
	types.append("jail");
	types.append("gotojail");
	types.append("todo");

	statusBar()->message(i18n("Welcome!"));
}

void TopLevel::openNew()
{
	estates.clear();

	for (int i = 1; i <= max; i++)
	{
		Estate estate;
		estate.name = QString(i18n("Estate %1").arg(i));
		estate.fg = white;
		estate.bg = white;
		estate.num = i;
		estate.type = 0;
		estates.append(estate);
	}
	// max blank estates...
	
	editor->setEstate(estates.first());
}

void TopLevel::open()
{
	filename = KFileDialog::getOpenFileName();
}

void TopLevel::save()
{
	if (filename == QString::null)
		filename = KFileDialog::getOpenFileName();

	KConfig *config = new KConfig(filename);
	for (int i = 0; i <= max; i++)
	{
		Estate estate = (*estates.at(i));
		config->setGroup(estate.name);
		kdDebug() << "type is " << estate.type << endl;
		config->writeEntry("type", *types.at(estate.type));
		config->writeEntry("color", estate.fg);
		config->writeEntry("bgcolor", estate.bg);
		config->writeEntry("num", estate.num);
	}

	config->sync();
}

void TopLevel::closeEvent(QCloseEvent *e)
{
	kapp->quit();
}

void TopLevel::changeEstate(int index)
{
	kdDebug() << index << endl;
	kdDebug() << "count is " << estates.count() << endl;
	Estate estate = editor->estate();
	(*estates.at(estate.num - 1)) = estate;

	estate = (*estates.at(index));
	editor->setEstate(estate);
}

#include "designer.moc"
