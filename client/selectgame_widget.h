#ifndef ATLANTIK_SELECTGAME_WIDGET_H
#define ATLANTIK_SELECTGAME_WIDGET_H

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>

#include <klistview.h>
#include <kpushbutton.h>

class SelectGame : public QWidget
{
Q_OBJECT

public:
	SelectGame(QWidget *parent, const char *name=0);

	void initPage();
		bool validateNext();
		QString hostToConnect() const;
		int portToConnect();

	public slots:
		void validateConnectButton();

		void slotGameListClear();
		void slotGameListAdd(QString gameId, QString gameType, QString description, QString players);
		void slotGameListEdit(QString gameId, QString gameType, QString description, QString players);
		void slotGameListDel(QString gameId);

	private slots:
		void connectPressed();

	signals:
		void joinGame(int gameId);
		void newGame(const QString &gameType);
//		void statusChanged();

private:
	QVBoxLayout *m_mainLayout;
	QLabel *status_label;
	KListView *m_gameList;
	KPushButton *m_connectButton;
};

#endif
