#ifndef ATLANTIK_NEWGAMEDLG_H
#define ATLANTIK_NEWGAMEDLG_H

#include <qlistview.h>
#include <qstring.h>
#include <qradiobutton.h>

#include <kwizard.h>
#include <kdialogbase.h>

#include "network.h"
#include "monopigator.h"

class SelectGame : public QWidget
{
Q_OBJECT

	public:
		SelectGame(QWidget *parent, const char *name=0);
		void initPage();
		void validateButtons();
		bool validateNext();
		int gameToJoin();

	public slots:
		void slotConnectionError(int);
		void slotConnected();

		void slotGamelistUpdate(QString);
		void slotGamelistEndUpdate(QString);
		void slotGamelistAdd(QString, QString);
		void slotGamelistEdit(QString, QString);
		void slotGamelistDel(QString);

		void slotInitPage();

		void slotGamelistClicked(QListViewItem *);

	signals:
		void statusChanged();

	private:
		QRadioButton *bnew, *bjoin;
		QListView *list;
		QLabel *status_label;
		GameNetwork *gameNetwork;
};

class ConfigureGame : public QWidget
{
Q_OBJECT

	public:
		ConfigureGame(QWidget *parent, const char *name=0);
		void initPage();
		bool validateNext();

	public slots:
		void slotPlayerlistUpdate(QString);
		void slotPlayerlistEndUpdate(QString);
		void slotPlayerlistAdd(QString, QString, QString);
		void slotPlayerlistEdit(QString, QString, QString);
		void slotPlayerlistDel(QString);

	signals:
		void statusChanged();

	private:
		QListView *list;
		QLabel *status_label;
		GameNetwork *gameNetwork;
};

class NewGameWizard : public KWizard
{
Q_OBJECT

	public:
		NewGameWizard(QWidget *parent, const char *name=0, bool modal=true, WFlags f=0);
		~NewGameWizard(void);

	public slots:
		void slotValidateNext();
		void slotInit(const QString &);

	private:
		QListView *list;
		SelectGame *select_game;
		ConfigureGame *configure_game;
		GameNetwork *gameNetwork;
};

#endif
