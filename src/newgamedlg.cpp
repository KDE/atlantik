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

#include "newgamedlg.moc"

NewGameDialog::NewGameDialog(QWidget *parent, const char *name = 0) : QWidget(parent, name)
{
//	connect(bcancel, SIGNAL(clicked()), parent, slot(slotCloseNewGameDlg()));

	setCaption("New game");
	QVBoxLayout *qbox = new QVBoxLayout(this);
	QLabel *aap = new QLabel(this);
//	aap->setText("select server\nconnect\nlistgames\noption for new game\n");
	aap->setText("Please select a server to connect to:\n(you should choose localhost for now)\n");
	qbox->addWidget(aap);
	
//	QListView *list = new QListView(this);
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

	QString title;
	title.setLatin1("Connect", strlen("Connect"));	
	QPushButton *bconnect = new QPushButton(title, this);
	qbox->addWidget(bconnect);
	title.setLatin1("Cancel", strlen("Cancel"));	
	QPushButton *bcancel = new QPushButton(title, this);
	qbox->addWidget(bcancel);

	connect(bconnect, SIGNAL(clicked()), this, SLOT(slotConnect()));
	connect(bcancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
	show();
}

void NewGameDialog::paintEvent(QPaintEvent *)
{
	QPainter painter;
	painter.drawRect(0,0,width(),3);
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
	// hide();
	delete this;
}
