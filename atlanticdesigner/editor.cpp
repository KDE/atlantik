#include <qcombobox.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <kcolorbutton.h>
#include <qlineedit.h>
#include <klocale.h>
#include <kdebug.h>
#include <qlayout.h>

#include <board.h>
#include "editor.h"

EstateEdit::EstateEdit(QWidget *parent, const char *name) : AtlantikBoard(parent, name)
{
	QVBoxLayout *hugeLayout = new QVBoxLayout(this);
	board = new AtlantikBoard(this, "Editing Board");
	hugeLayout->addWidget(board);
	centerWidget = board->centerWidget();

	QVBoxLayout *layout = new QVBoxLayout(centerWidget, 6);
	nameEdit = new QLineEdit(centerWidget, "Name Edit");
	layout->addWidget(nameEdit);
	
	QHBoxLayout *colorLayout = new QHBoxLayout(layout, 6);
	fgButton = new KColorButton(centerWidget, "Foreground Button");
	colorLayout->addWidget(fgButton);
	bgButton = new KColorButton(centerWidget, "Background Button");
	colorLayout->addWidget(bgButton);
	//layout->addStretch();

	QHBoxLayout *typeLayout = new QHBoxLayout(layout, 6);
	QLabel *typeLabel = new QLabel(i18n("Type"), centerWidget);
	typeLayout->addWidget(typeLabel);
	typeCombo = new QComboBox(false, centerWidget, "Type Combo");
	typeLayout->addWidget(typeCombo);

	QStringList types(i18n("Street"));
	types.append(i18n("Airport"));
	types.append(i18n("Chance"));
	types.append(i18n("Street"));
	types.append(i18n("Go"));
	types.append(i18n("Jail"));
	types.append(i18n("Go to jail"));
	types.append(i18n("TODO"));
	typeCombo->insertStringList(types);
}

void EstateEdit::setEstate(Estate estate)
{
	typeCombo->setCurrentItem(estate.type);
	nameEdit->setText(estate.name);
	fgButton->setColor(estate.fg);
	bgButton->setColor(estate.bg);
	num = estate.num;
	kdDebug() << "num is now " << num << endl;
}

Estate EstateEdit::estate()
{
	Estate ret;
	ret.type = typeCombo->currentItem();
	kdDebug() << "ret.type = " << ret.type << endl;
	ret.name = nameEdit->text();
	ret.fg = fgButton->color();
	ret.bg = bgButton->color();
	ret.num = num;

	return ret;
}

#include "editor.moc"
