#include <kmainwindow.h>
#include <qptrlist.h>
#include <qtimer.h>
#include <kglobal.h>
#include <klocale.h>
#include <qfile.h>
#include <qtextstream.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <qwidget.h>
#include <kmessagebox.h>
#include <qstring.h>
#include <kapp.h>
#include <qevent.h>
#include <qcolor.h>
#include <kdebug.h>
#include <qlayout.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kstandarddirs.h>

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

	m_player = 0;

	(void) KStdAction::close(this, SLOT(close()), actionCollection());
	(void) KStdAction::open(this, SLOT(open()), actionCollection());
	(void) KStdAction::openNew(this, SLOT(openNew()), actionCollection());
	(void) KStdAction::save(this, SLOT(save()), actionCollection());
	(void) KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());

	(void) new KAction(i18n("&Larger"), "zoom+", 0, this, SLOT(larger()), actionCollection(), "larger");
	(void) new KAction(i18n("&Smaller"), "zoom-", 0, this, SLOT(smaller()), actionCollection(), "smaller");

	estateAct = new KListAction(i18n("Change estate"), 0, 0, 0, actionCollection(), "estate_num");
	QStringList estates;
	for (int i = 1; i <= max; i++)
		estates.append(i18n("Jump to estate %1").arg(QString::number(i)));
	estateAct->setItems(estates);
	connect(estateAct, SIGNAL(activated(int)), SLOT(changeEstate(int)));

	createGUI("designerui.rc");

	// these MUST match up to the ones in editor.cpp!
	types.append("street");
	types.append("rr");
	types.append("utility");
	types.append("cc");
	types.append("chance");
	types.append("freeparking");
	types.append("tojail");
	types.append("tax");
	types.append("airport");
	types.append("jail");
	types.append("go");
	types.append("TODO");

	isMod = false;
	openNew();

	doCaption(false);

	resize(350, 400);
	applyMainWindowSettings(KGlobal::config(), "DesignerTopLevelWindow");
}

AtlanticDesigner::~AtlanticDesigner()
{
	delete m_player;
}

void AtlanticDesigner::openNew()
{
	if (warnClose())
		return;
	filename = QString::null;
	KStandardDirs *dirs = KGlobal::dirs();
	openFile(dirs->findResource("appdata", "defaultcity.conf"));
}

bool AtlanticDesigner::warnClose()
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
			return false;
		case KMessageBox::Cancel:
			return true;
		}
	}
	return false;
}

void AtlanticDesigner::open()
{
	if (warnClose())
		return;
	filename = KFileDialog::getOpenFileName();
	openFile(filename);
}

void AtlanticDesigner::openFile(const QString &filename)
{
	estates.clear();

	QFile f(filename);
	if (!f.open(IO_ReadOnly))
		return;

	QTextStream t(&f);
	QString s;

	for (int i = 0; !t.atEnd(); i++)
	{
		QString tmps = (i == 0? t.readLine().stripWhiteSpace() : s);
		QString name;
		name = tmps.left(tmps.find("]"));
		name = name.right(name.length() - name.find("[") - 1);
		if (name.isEmpty())
			name = i18n("Empty");
		kdDebug() << "opening " << name << endl;
		//kdDebug() << "name is " << name << endl;;
		QColor color = QColor("zzzzzz"), bgColor = QColor("zzzzzz");
		int type = 0;
		int group = -1;
		int price = -1;
		int housePrice = -1;
		int rent[6] = {-1, -1, -1, -1, -1, -1};
		int tax = -1;
		int taxPercentage = -1;

		while (true)
		{
			if (t.atEnd())
				break;
			s = t.readLine().stripWhiteSpace();

			if (s.left(1) == "[")
			{
				break;
			}

			int eqSign = s.find("=");
			if (eqSign == -1)
				continue;

			QString key = s.left(eqSign);
			QString value = s.right(s.length() - eqSign - 1);

			if (key == "type")
			{
				//kdDebug() << "its a type!\n";
				int i = 0;
				for (QStringList::Iterator it = types.begin(); it != types.end(); ++it)
				{
					//kdDebug() << (*it) << ", " << value << endl;
					if ((*it) == value)
					{
						type = i;
						break;
					}
					i++;
				}
				//kdDebug() << "type is " << type << endl;
			}
			else if (key == "price")
			{
				price = value.toInt();
			}
			else if (key == "houseprice")
			{
				housePrice = value.toInt();
			}
			else if (key.left(4) == "rent")
			{
				int houses = key.right(1).toInt();
				if (houses < 0 || houses > 5)
					continue;
				rent[houses] = value.toInt();
			}
			else if (key == "tax")
			{
				tax = value.toInt();
			}
			else if (key == "taxpercentage")
			{
				taxPercentage = value.toInt();
			}
			else if (key == "group")
			{
				group = value.toInt();
			}
			else if (key == "color")
			{
				color.setNamedColor(value);
				//kdDebug() << "color is " << color.name() << endl;
			}
			else if (key == "bgcolor")
			{
				bgColor.setNamedColor(value);
				//kdDebug() << "bgcolor is " << bgColor.name() << endl;
			}
		}

		ConfigEstate *estate = new ConfigEstate(i);
		estate->setName(name);
		estate->setColor(color);
		estate->setBgColor(bgColor);
		estate->setType(type);
		estate->setGroup(group);
		estate->setPrice(price);
		estate->setHousePrice(housePrice);
		for (int i = 0; i < 6; i++)
			estate->setRent(i, rent[i]);
		estate->setTax(tax);
		estate->setTaxPercentage(taxPercentage);
		estate->setChanged(false);
		estates.append(estate);

		connect(estate, SIGNAL(LMBClicked(Estate *)), this, SLOT(changeEstate(Estate *)));
		connect(estate, SIGNAL(changed()), this, SLOT(modified()));

		editor->addEstateView(estate);
	}
	
	editor->setEstate(estates.first());

	// our superstar!
	delete m_player;
	m_player = new Player(1);
	editor->addToken(m_player);
	QTimer::singleShot(1000, this, SLOT(setPlayerAtBeginning()));

	isMod = false;
	doCaption(false);
}

void AtlanticDesigner::setPlayerAtBeginning()
{
	movePlayer(estates.first());
}

void AtlanticDesigner::saveAs()
{
	QString oldfilename = filename;
	filename = QString::null;
	save();
	if (filename == QString::null)
		filename = oldfilename;
}

void AtlanticDesigner::save()
{
	//kdDebug() << "count is " << estates.count() << endl;
	if (filename == QString::null)
		filename = KFileDialog::getOpenFileName();

	if (filename == QString::null)
		return;

	//QStringList allNames;

	QFile f(filename);
	if (!f.open(IO_WriteOnly))
		return;

	QTextStream t(&f);

	ConfigEstate *estate = 0;
	for (estate = estates.first(); estate; estate = estates.next())
	{
		/*
		if (!allNames.grep(estate->name()).empty())
		{
			KMessageBox::detailedSorry(this, i18n("There are duplicate names on your gameboard. Thus, it can not be saved correctly; aborting."), i18n("%1 (number %1) has a duplicate name.").arg(estate->name()).arg(estate->estateId()));
			return;
		}
		*/

		t << QString("[%1]\ntype=%2\nbgcolor=%4\n").arg(estate->name()).arg(*types.at(estate->type())).arg(estate->bgColor().name());
		if (estate->color().isValid())
			t << "color=" << estate->color().name() << endl;
		if (estate->group() >= 0)
			t << "group=" << estate->group() << endl;
		if (estate->price() >= 0)
			t << "price=" << estate->price() << endl;
		if (estate->tax() >= 0)
			t << "tax=" << estate->tax() << endl;
		if (estate->taxPercentage() >= 0)
			t << "taxpercentage=" << estate->taxPercentage() << endl;

		for (int i = 0; i < 6; i++)
		{
			if (estate->rent(i) >= 0)
				t << "rent" << i << "=" << estate->rent(i) << endl;
		}

		//allNames.append(estate->name());

		kdDebug() << "done with " << estate->name() << endl;
	}

	f.flush();
	isMod = false;
	doCaption(false);
}

void AtlanticDesigner::closeEvent(QCloseEvent *e)
{
	if (warnClose())
		return;
	saveMainWindowSettings(KGlobal::config(), "DesignerTopLevelWindow");
	e->accept();
}

void AtlanticDesigner::changeEstate(int index)
{
	//kdDebug() << index << endl;
	//kdDebug() << "count is " << estates.count() << endl;

	(void) editor->saveEstate();

	editor->setEstate(estates.at(index));
	movePlayer(estates.at(index + 1));
}

void AtlanticDesigner::changeEstate(Estate *estate)
{
	if (!estate)
		return;

	(void) editor->saveEstate();

	editor->setEstate(static_cast<ConfigEstate *>(estate));
	movePlayer(estate);
}

void AtlanticDesigner::movePlayer(Estate *estate)
{
	m_player->setLocation(estate);
	m_player->update();

	estateAct->setCurrentItem(estate->estateId() - 1);
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
