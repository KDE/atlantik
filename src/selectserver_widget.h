#include <qwidget.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "monopigator.h"
#include "network.h"

class SelectServer : public QWidget
{
Q_OBJECT

public:
	SelectServer(QWidget *parent, const char *name=0);

	void initPage();
		bool validateNext();
		QString hostToConnect() const;
		int portToConnect();

	public slots:
		void initMonopigator();

		void validateRadioButtons();
		void validateConnectButton();

		void slotMonopigatorClear();
		void slotMonopigatorAdd(QString host, QString port, QString version);
		void slotListClicked(QListViewItem *);

	private slots:
		void connectPressed();
		void monopigatorFinished();

	signals:
		void serverConnect(const QString host, int port);
//		void statusChanged();

	private:
		QVBoxLayout *m_mainLayout;
		QLabel *status_label;
		QRadioButton *m_localGameButton, *m_onlineGameButton;
		QListView *m_serverList;
		QPushButton *m_refreshButton, *m_connectButton;
		Monopigator *monopigator;
		GameNetwork *gameNetwork;
};
