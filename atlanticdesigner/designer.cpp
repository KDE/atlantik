#include <kmainwindow.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <qwidget.h>
#include <kmessagebox.h>
#include <kapp.h>
#include <qevent.h>
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
	editor = new EstateEdit(this, "Estate Editor");
	setCentralWidget(editor);

	(void) KStdAction::quit(this, SLOT(close()), actionCollection());
	(void) KStdAction::open(this, SLOT(close()), actionCollection());
	(void) KStdAction::openNew(this, SLOT(close()), actionCollection());
	(void) KStdAction::save(this, SLOT(close()), actionCollection());

	createGUI();

	statusBar();
}

void TopLevel::open()
{
}

void TopLevel::openNew()
{
}

void TopLevel::save()
{
}

void TopLevel::closeEvent(QCloseEvent *e)
{
	kapp->quit();
}
