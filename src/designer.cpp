#include <kmainwindow.h>
#include <qptrlist.h>
#include <kglobal.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <qwidget.h>
#include <kmessagebox.h>
#include <qstring.h>
#include <kapp.h>
#include <kconfig.h>
#include <qevent.h>
#include <qcolor.h>
#include <kdebug.h>
#include <qlayout.h>
#include <kaction.h>
#include <kstdaction.h>

#include "player.h"
#include "designer.h"
#include "estate.h"
#include "editor.h"

AtlanticDesigner::AtlanticDesigner(QWidget *parent, const char *name) : KMainWindow(parent, name)
{
	max = 40;
	estates.setAutoDelete(true);

	editor = new EstateEdit(this, "Estate Editor");
	setCentralWidget(editor);

	// our superstar!
	player = new Player(1);
	editor->addToken(player);

	(void) KStdAction::close(this, SLOT(close()), actionCollection());
	(void) KStdAction::open(this, SLOT(open()), actionCollection());
	(void) KStdAction::openNew(this, SLOT(openNew()), actionCollection());
	(void) KStdAction::save(this, SLOT(save()), actionCollection());

	(void) new KAction(i18n("&Larger"), "zoom+", 0, this, SLOT(larger()), actionCollection(), "larger");
	(void) new KAction(i18n("&Smaller"), "zoom-", 0, this, SLOT(smaller()), actionCollection(), "smaller");

	estateAct = new KListAction(i18n("Change estate"), 0, 0, 0, actionCollection(), "estate_num");
	QStringList estates;
	for (int i = 1; i <= max; i++)
		estates.append(i18n("Jump to estate %1").arg(QString::number(i)));
	estateAct->setItems(estates);
	connect(estateAct, SIGNAL(activated(int)), SLOT(changeEstate(int)));

	createGUI("designerui.rc");

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

	doCaption(false);

	resize(350, 400);
	applyMainWindowSettings(KGlobal::config(), "DesignerTopLevelWindow");
	isMod = false;
}

AtlanticDesigner::~AtlanticDesigner()
{
	delete player;
}

void AtlanticDesigner::openNew()
{
	estates.clear();

	for (int i = 1; i <= max; i++)
	{
		ConfigEstate *estate = new ConfigEstate(i);
		estate->setName(i18n("Estate %1").arg(i));
		estate->setColor(white);
		estate->setBgColor(white);
		estate->setType(0);
		estates.append(estate);

		connect(estate, SIGNAL(LMBClicked(Estate *)), this, SLOT(changeEstate(Estate *)));
		connect(estate, SIGNAL(changed()), this, SLOT(modified()));

		editor->addEstateView(estate);
	}
	// max blank estates...
	
	editor->setEstate(estates.first());

	filename = QString::null;
	isMod = false;
}

void AtlanticDesigner::open()
{
	filename = KFileDialog::getOpenFileName();
}

void AtlanticDesigner::save()
{
	kdDebug() << "count is " << estates.count() << endl;
	if (filename == QString::null)
		filename = KFileDialog::getOpenFileName();

	if (filename == QString::null)
		return;

	QStringList allNames;

	ConfigEstate *estate = 0;
	KConfig *config = new KConfig(filename);
	for (estate = estates.first(); estate; estate = estates.next())
	{
		if (!allNames.grep(estate->name()).empty())
		{
			KMessageBox::detailedSorry(this, i18n("There are duplicate names on your gameboard. Thus, it can not be saved correctly; aborting."), i18n("%1 (number %1) has a duplicate name.").arg(estate->name()).arg(estate->estateId()));
			return;
		}
		config->setGroup(estate->name());
		kdDebug() << "type is " << estate->type() << endl;
		config->writeEntry("type", (*types.at(estate->type())));
		config->writeEntry("color", estate->color());
		config->writeEntry("bgcolor", estate->bgColor());

		allNames.append(estate->name());

		kdDebug() << "done with " << estate->name() << endl;
	}

	config->sync();

	isMod = false;
}

void AtlanticDesigner::closeEvent(QCloseEvent *e)
{
	if (isMod)
	{
		int result = KMessageBox::warningYesNoCancel(this, i18n("There are unsaved changes to gameboard. Save them?"), i18n("Unsaved changes"), i18n("Save"), i18n("Discard"), "DiscardAsk", true);
		switch(result)
		{
		case KMessageBox::Yes:
			save();
			// fallthrough
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return;
		}
	}

	saveMainWindowSettings(KGlobal::config(), "DesignerTopLevelWindow");
	e->accept();
}

void AtlanticDesigner::changeEstate(int index)
{
	kdDebug() << index << endl;
	kdDebug() << "count is " << estates.count() << endl;

	(void) editor->saveEstate();

	editor->setEstate(estates.at(index));
	movePlayer(index + 1);
}

void AtlanticDesigner::changeEstate(Estate *estate)
{
	if (!estate)
		return;

	(void) editor->saveEstate();

	editor->setEstate(static_cast<ConfigEstate *>(estate));
	movePlayer(estate->estateId());
}

void AtlanticDesigner::movePlayer(int estateId)
{
	editor->slotMsgPlayerUpdateLocation(1, estateId, false);
	editor->raiseToken(1);
	editor->slotMoveToken();

	estateAct->setCurrentItem(estateId - 1);
}

void AtlanticDesigner::larger()
{
	// this will add a square to all sides
}

void AtlanticDesigner::smaller()
{
	// this will remove a square from all sides
}

void AtlanticDesigner::modified()
{
	isMod = true;
	doCaption(true);
}

void AtlanticDesigner::doCaption(bool modified)
{
	setCaption(i18n("Atlantic gameboard editor"), modified);
}

#include "designer.moc"
