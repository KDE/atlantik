#include <qwidget.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "network.h"

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
		void slotGameListAdd(QString gameId, QString description, QString players);
		void slotGameListEdit(QString gameId, QString description, QString players);
		void slotGameListDel(QString gameId);

	private slots:
		void connectPressed();

	signals:
		void gameConnect(const QString host, int port);
//		void statusChanged();

	private:
		QVBoxLayout *m_mainLayout;
		QLabel *status_label;
		QListView *m_gameList;
		QPushButton *m_refreshButton, *m_connectButton;
//		GameNetwork *gameNetwork;
};
