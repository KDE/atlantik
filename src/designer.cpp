#include <qcolor.h>
#include <qevent.h>
#include <qfile.h>
#include <qguardedptr.h>
#include <qlayout.h>
#include <qpoint.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qwidget.h>

#include <kaction.h>
#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstdaction.h>
#include <kurl.h>

#include "player.h"
#include "designer.h"
#include "estate.h"
#include "editor.h"
#include "board.h"

AtlanticDesigner::AtlanticDesigner(QWidget *parent, const char *name) : KMainWindow(parent, name)
{
	estates.setAutoDelete(true);
	//chanceStack.setAutoDelete(true);
	//ccStack.setAutoDelete(true);
	
	firstBoard = true;

	m_player = 0;
	copiedEstate = 0;
	editor = 0;
	board = 0;
	layout = 0;

	(void) KStdAction::close(this, SLOT(close()), actionCollection());
	(void) KStdAction::open(this, SLOT(open()), actionCollection());
	(void) KStdAction::openNew(this, SLOT(openNew()), actionCollection());
	(void) KStdAction::save(this, SLOT(save()), actionCollection());
	(void) KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());

	(void) new KAction(i18n("&Larger"), "zoom+", 0, this, SLOT(larger()), actionCollection(), "larger");
	(void) new KAction(i18n("&Smaller"), "zoom-", 0, this, SLOT(smaller()), actionCollection(), "smaller");
	(void) KStdAction::copy(this, SLOT(copy()), actionCollection());
	(void) KStdAction::paste(this, SLOT(paste()), actionCollection());
	(void) new KAction(i18n("&Up"), Key_Up, this, SLOT(up()), actionCollection(), "up");
	(void) new KAction(i18n("&Down"), Key_Down, this, SLOT(down()), actionCollection(), "down");
	(void) new KAction(i18n("&Left"), Key_Left, this, SLOT(left()), actionCollection(), "left");
	(void) new KAction(i18n("&Right"), Key_Right, this, SLOT(right()), actionCollection(), "right");

	recentAct = KStdAction::openRecent(0, 0, actionCollection());
	connect(recentAct, SIGNAL(urlSelected(const KURL &)), this, SLOT(openRecent(const KURL &)));
	recentAct->loadEntries(KGlobal::config(), "Designer recent files");

	estateAct = new KListAction(i18n("Change estate"), 0, 0, 0, actionCollection(), "estate_num");
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

void AtlanticDesigner::initBoard()
{
	// let her say her prayers (if she's alive)
	if (!firstBoard)
		editor->aboutToDie();

	firstBoard = false;

	delete editor;
	delete board;
	delete layout;

	estates.clear();
	chanceStack.setAutoDelete(true);
	chanceStack.clear();
	chanceStack.setAutoDelete(false);
	ccStack.setAutoDelete(true);
	ccStack.clear();
	ccStack.setAutoDelete(false);

	board = new AtlantikBoard(this, "Board");
	setCentralWidget(board);
	layout = new QVBoxLayout(board->centerWidget());
	editor = new EstateEdit(&chanceStack, &ccStack, board->centerWidget(), "Estate Editor");
	layout->addWidget(editor);

	editor->setReady(false);

	board->show();
	editor->show();
}

void AtlanticDesigner::openNew()
{
	if (warnClose())
		return;
	filename = QString::null;

	initBoard();

	KConfig *config = kapp->config();
	QColor fg, bg;
	config->setGroup("General");
	bg = config->readColorEntry("alternateBackground", &black);
	config->setGroup("WM");
	fg = config->readColorEntry("activeBackground", &black);

	for(int i = 0; i < 40; ++i)
	{
		ConfigEstate *estate = new ConfigEstate(i);
		estate->setName(i18n("New Estate"));
		estate->setColor(fg);
		estate->setBgColor(bg);
		estate->setPrice(100);
		estate->setHousePrice(50);
		for (int i = 0; i < 6; ++i)
			estate->setRent(i, 10 * (i + 1));
		estate->setChanged(false);
		estates.append(estate);
	
		connect(estate, SIGNAL(LMBClicked(Estate *)), this, SLOT(changeEstate(Estate *)));
		connect(estate, SIGNAL(changed()), this, SLOT(modified()));
	
		board->addEstateView(estate);
	}

	max = 40;

	isMod = false;
	doCaption(false);
	updateJumpMenu();

	QTimer::singleShot(500, this, SLOT(setPlayerAtBeginning()));
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

	if (filename.isNull())
		return;

	openFile(filename);
	recentAct->addURL(QString("file:") + filename);
}

void AtlanticDesigner::openRecent(const KURL &url)
{
	if (url.isLocalFile())
	{
		filename = url.path();
		openFile(filename);
	}
	else
	{
		recentAct->removeURL(url);
	}
}

void AtlanticDesigner::openFile(const QString &filename)
{
	enum ParseMode { Estates=0, Chance_Cards=1, CC_Cards=2, Other=3 };
	QFile f(filename);
	if (!f.open(IO_ReadOnly))
		return;
	
	initBoard();

	QTextStream t(&f);
	QString s = t.readLine();
	ParseMode parseMode = Other;

	int i;
	for (i = 0; !t.atEnd();)
	{
		if (i < 500)
			kdDebug() << "s is [" << s << "]" << endl;
		s = s.stripWhiteSpace();

		if (s.isEmpty())
		{
			kdDebug() << "thats empty, reading new line" << endl;
			s = t.readLine();
			continue;
		}

		QString name;

		if (s.left(1) == "<")
		{
			kdDebug() << "setting parsemode for s [" << s << "]" << endl;
			if (s == "<Estates>")
				parseMode = Estates;
			else if (s == "<Chance_Cards>")
				parseMode = Chance_Cards;
			else if (s == "<CC_Cards>")
				parseMode = CC_Cards;
			else
				parseMode = Other;

			s = t.readLine();
			continue;
		}

		name = s.left(s.find("]"));
		name = name.right(name.length() - name.find("[") - 1);
		
		if (name.isEmpty())
			name = i18n("No Name");
		kdDebug() << "name is " << name << endl;

		//// for estates
		QColor color = QColor("zzzzzz"), bgColor = QColor("zzzzzz");
		int type = 0;
		int group = -1;
		int price = -1;
		int housePrice = -1;
		int rent[6] = {-1, -1, -1, -1, -1, -1};
		int tax = -1;
		int taxPercentage = -1;
		//// for cards
		QStringList keys;
		QValueList<int> values;

		while (true)
		{
			if (t.atEnd())
				break;
			s = t.readLine().stripWhiteSpace();

			kdDebug() << "s is " << s << endl;

			if (s.left(1) == "[" || s.left(1) == "<")
			{
				kdDebug() << "breaking\n";
				break;
			}

			int eqSign = s.find("=");
			if (eqSign == -1)
			{
				continue;
			}

			QString key = s.left(eqSign);
			QString value = s.right(s.length() - eqSign - 1);

			//////////////////////////////// ESTATES
			if (parseMode == Estates)
			{
			if (key == "type")
			{
				//kdDebug() << "its a type!\n";
				int j = 0;
				for (QStringList::Iterator it = types.begin(); it != types.end(); ++it)
				{
					//kdDebug() << (*it) << ", " << value << endl;
					if ((*it) == value)
					{
						type = j;
						break;
					}
					j++;
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
				{
					continue;
				}
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
			} ///////////////////////////// END ESTATES

			  ///////////////////////////// CARDS
			else if (parseMode == Chance_Cards || parseMode == CC_Cards) 
			{
				kdDebug() << "in card area\n";
				bool ok;
				int v = value.toInt(&ok);
				if (!ok)
					continue;

				if (key == "pay" && v < 0)
				{
					v *= -1;
					key = "collect";
				}
				if (key == "payeach" && v < 0)
				{
					v *= -1;
					key = "collecteach";
				}
				if (key == "advance" && v < 0)
				{
					v *= -1;
					key = "goback";
				}
				if (key == "advanceto")
					v++;

				kdDebug() << key << "=" << v << endl;
				keys.append(key);
				values.append(v);
			}
			else
				kdDebug() << "ignoring line, unknown parseMode" << endl;
		}

		if (parseMode == Estates)
		{
			kdDebug() << "making estate (" << i << ")\n";
			ConfigEstate *estate = new ConfigEstate(i);
			estate->setName(name);
			estate->setColor(color);
			estate->setBgColor(bgColor);
			estate->setType(type);
			estate->setGroup(group);
			estate->setPrice(price);
			estate->setHousePrice(housePrice);
			for (int j = 0; j < 6; j++)
				estate->setRent(j, rent[j]);
			estate->setTax(tax);
			estate->setTaxPercentage(taxPercentage);
			estate->setChanged(false);
			estates.append(estate);
	
			connect(estate, SIGNAL(LMBClicked(Estate *)), this, SLOT(changeEstate(Estate *)));
			connect(estate, SIGNAL(changed()), this, SLOT(modified()));
	
			kdDebug() << "addEstateView\n";
			board->addEstateView(estate);

			kdDebug() << "incrementing i\n";
			i++;
		}
		else if (parseMode == Chance_Cards || parseMode == CC_Cards)
		{
			kdDebug() << "making new card" << endl;
			Card *card = new Card();
			card->name = name;
			card->keys = keys;
			card->values = values;
			(parseMode == Chance_Cards ? chanceStack : ccStack).append(card);
		}
	}

	if (i < 4)
	{
			KMessageBox::detailedSorry(this, i18n("This board file is bad; cannot open."), i18n("There are only %1 estates specified in this file.").arg(i));
			if (this->filename.isNull())
				close();
			return;
	}

	max = i;

	isMod = false;
	doCaption(false);
	updateJumpMenu();
	kdDebug() << "ending openFile\n";

	QTimer::singleShot(500, this, SLOT(setPlayerAtBeginning()));
}

void AtlanticDesigner::updateJumpMenu()
{
	QStringList estates;
	for (int i = 1; i <= max; i++)
		estates.append(i18n("Jump to estate %1").arg(QString::number(i)));
	estateAct->setItems(estates);
}

void AtlanticDesigner::setPlayerAtBeginning()
{
	delete m_player;
	// our superstar!
	m_player = new Player(1);
	board->addToken(m_player);
	movePlayer(estates.first());
	editor->setEstate(estates.first());
}

void AtlanticDesigner::saveAs()
{
	QString oldfilename = filename;
	filename = QString::null;
	save();
	if (filename.isNull())
		filename = oldfilename;
	else
		recentAct->addURL(QString("file:") + filename);
}

void AtlanticDesigner::save()
{
	(void) editor->saveEstate();
	//kdDebug() << "count is " << estates.count() << endl;
	QString oldfilename = filename;
	if (filename.isNull())
		filename = KFileDialog::getOpenFileName();

	if (filename.isNull())
	{
		filename = oldfilename;
		return;
	}

	//QStringList allNames;

	QFile f(filename);
	if (!f.open(IO_WriteOnly))
		return;

	QTextStream t(&f);

	t << QString("<Estates>") << endl << endl;

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

		EstateType type = (EstateType)estate->type();

		t << QString("[%1]\ntype=%2\nbgcolor=%4\n").arg(estate->name()).arg(*types.at(estate->type())).arg(estate->bgColor().name());
		if (estate->color().isValid())
			t << "color=" << estate->color().name() << endl;
		if (estate->group() >= 0 && type == Street)
			t << "group=" << estate->group() << endl;
		if (estate->housePrice() >= 0 && type == Street)
			t << "houseprice=" << estate->housePrice() << endl;
		if (estate->price() >= 0 && type == Street)
			t << "price=" << estate->price() << endl;
		if (estate->tax() >= 0 && type == Tax)
			t << "tax=" << estate->tax() << endl;
		if (estate->taxPercentage() >= 0 && type == Tax)
			t << "taxpercentage=" << estate->taxPercentage() << endl;

		if (type == Street)
			for (int i = 0; i < 6; i++)
				if (estate->rent(i) >= 0)
					t << "rent" << i << "=" << estate->rent(i) << endl;

		t << endl;

		//allNames.append(estate->name());
	}

	// now do the cards
	
	CardStack *stack = 0;
	for (int i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			t << endl << QString("<Chance_Cards>") << endl;
			stack = &chanceStack;
		}
		else if (i == 1)
		{
			t << endl << QString("<CC_Cards>") << endl;
			stack = &ccStack;
		}

		Card *card = 0;
		for (card = stack->first(); card; card = stack->next())
		{
			t << endl << "[" << card->name << "]" << endl;
			QValueList<int>::Iterator vit = card->values.begin();
			for (QStringList::Iterator it = card->keys.begin(); it != card->keys.end(); ++it)
			{
				QString key = (*it);
				int value = (*vit);

				if (key == "collect")
				{
					value *= -1;
					key = "pay";
				}
				if (key == "collecteach")
				{
					value *= -1;
					key = "payeach";
				}
				if (key == "goback")
				{
					value *= -1;
					key = "advance";
				}
				if (key == "advanceto")
					value--;

				if (key == "jailcard" || key == "tojail" || key == "nextrr" || key == "nextutil")
				{
					value = 1;
				}
				
				t << key << "=" << value << endl;

				++vit;
			}
		}
	}

	f.flush();
	isMod = false;
	doCaption(false);
}

void AtlanticDesigner::copy()
{
	copiedEstate = editor->theEstate();
}

void AtlanticDesigner::paste()
{
	ConfigEstate *estate = editor->theEstate();
	estate->setColor(copiedEstate->color());
	estate->setBgColor(copiedEstate->bgColor());
	estate->setType(copiedEstate->type());
	estate->setGroup(copiedEstate->group());
	for (int i = 0; i < 6; i++)
		estate->setRent(i, copiedEstate->rent(i));
	estate->setPrice(copiedEstate->price());
	estate->setHousePrice(copiedEstate->housePrice());
	estate->setTax(copiedEstate->tax());
	estate->setTaxPercentage(copiedEstate->taxPercentage());

	editor->setEstate(estate);
}

void AtlanticDesigner::closeEvent(QCloseEvent *e)
{
	if (warnClose())
		return;
	saveMainWindowSettings(KGlobal::config(), "DesignerTopLevelWindow");
	recentAct->saveEntries(KGlobal::config(), "Designer recent files");
	e->accept();
}

void AtlanticDesigner::changeEstate(int index)
{
	//kdDebug() << index << endl;
	//kdDebug() << "count is " << estates.count() << endl;

	(void) editor->saveEstate();

	editor->setEstate(estates.at(index));
	movePlayer(estates.at(index));
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
	estateAct->setCurrentItem(estate->estateId());
	board->setFocus();
	m_player->setLocation(estate);
	m_player->update();
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

// now some fun functions ;)

void AtlanticDesigner::up()
{
	if (editor->upArrow())
		return;
	int fourth = max / 4;
	int estateId = editor->theEstate()->estateId() + 1;
	int dest = estateId - 1;

	if (estateId <= 2*fourth && estateId > fourth) // left side
		dest++;
	else if (estateId > (3*fourth + 1)) // right side
		dest--;
	else if (estateId == 1)
		dest = max - 1;

	changeEstate(dest);
}

void AtlanticDesigner::down()
{
	if (editor->downArrow())
		return;
	int fourth = max / 4;
	int estateId = editor->theEstate()->estateId() + 1;
	int dest = estateId - 1;

	if (estateId <= (2*fourth + 1) && estateId > (fourth + 1)) // left side
		dest--;
	else if (estateId > 3*fourth && estateId < max) // right side
		dest++;
	else if (estateId == max)
		dest = 0;

	changeEstate(dest);
}

void AtlanticDesigner::left()
{
	if (editor->leftArrow())
		return;
	int fourth = max / 4;
	int estateId = editor->theEstate()->estateId() + 1;
	int dest = estateId - 1;

	if (estateId <= fourth) // bottom
		dest++;
	else if (estateId > (2*fourth + 1) && estateId <= (3*fourth + 1)) // top
		dest--;

	changeEstate(dest);
}

void AtlanticDesigner::right()
{
	if (editor->rightArrow())
		return;
	int fourth = max / 4;
	int estateId = editor->theEstate()->estateId() + 1;
	int dest = estateId - 1;

	if (estateId <= (fourth + 1) && estateId != 1) // bottom
		dest--;
	else if (estateId > 2*fourth && estateId <= 3*fourth) // top
		dest++;

	changeEstate(dest);
}

#include "designer.moc"
