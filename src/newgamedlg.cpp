#include <qlabel.h>

#include <qcolor.h>
#include <qimage.h>
#include <qpainter.h>

#include <qtextview.h>

#include <qlayout.h>
#include <iostream.h> // cout etc
#include <qlineedit.h>
#include <qstring.h>

#include <qcstring.h>
#include <qsocket.h>

#include <kstdaction.h>
#include <kaction.h>
#include <kstdaccel.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kapp.h>

#include <kbuttonbox.h>
#include <klocale.h>

#include "newgamedlg.moc"

NewGameDialog::NewGameDialog(QWidget *parent, const char *name = 0, bool modal = true) : KDialog( parent, name, modal )
{
	QVBoxLayout *qbox = new QVBoxLayout(this, marginHint(), spacingHint());
	CHECK_PTR(qbox);
	
	QLabel *header_label = new QLabel(this);
	/* steps: select server, connect, listgames, join/create, config/wait, play */
	header_label->setText("Please select a server to connect to:\n(you should choose localhost for now)\n");
	qbox->addWidget(header_label);

	list = new QListView(this);

	QListViewItem *item;
	item = new QListViewItem(list, "localhost", "1234", "0");
//	item = new QListViewItem(list, "monopd.capsi.com", "1234", "0");

	QString column;
	column.setLatin1("Server", strlen("Server"));
	list->addColumn(column);
	column.setLatin1("Port", strlen("Port"));
	list->addColumn(column);
	column.setLatin1("Users", strlen("Users"));
	list->addColumn(column);

	qbox->addWidget(list);

	KButtonBox *bbox = new KButtonBox( this, KButtonBox::Horizontal, 0, spacingHint());
	CHECK_PTR( bbox );
	qbox->addWidget(bbox);

	bbox->addStretch();

	bconnect = bbox->addButton( i18n("&Connect"), false );
	bcancel = bbox->addButton( i18n("C&ancel"), false );

	bbox->layout();
	bconnect->setDefault(true);
	
	connect(bconnect, SIGNAL(clicked()), this, SLOT(slotConnect()));
	connect(bcancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
}

NewGameDialog::~NewGameDialog()
{
}

void NewGameDialog::slotConnect()
{
	cout << "bconnect clicked" << endl;
	QListViewItem *item;
	if ((item = list->selectedItem()))
	{
		cout << "item selected" << endl;
	}
	else
		cout << "nothing selected" << endl;
}

void NewGameDialog::slotCancel()
{
	cout << "bcancel clicked" << endl;
}
