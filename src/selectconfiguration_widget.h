#include <qwidget.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlabel.h>

class SelectConfiguration : public QWidget
{
Q_OBJECT

public:
	SelectConfiguration(QWidget *parent, const char *name=0);

	void initPage();
		bool validateNext();
		QString hostToConnect() const;
		int portToConnect();

	public slots:
		void slotPlayerListClear();
		void slotPlayerListAdd(QString playerId, QString name, QString host);
		void slotPlayerListEdit(QString playerId, QString name, QString host);
		void slotPlayerListDel(QString playerId);

	private slots:
		void connectPressed();
		void slotClicked();

	signals:
		void startGame();
		void joinConfiguration(int configurationId);
		void newConfiguration();
//		void statusChanged();

	private:
		QVBoxLayout *m_mainLayout;
		QLabel *status_label;
		QVGroupBox *m_playerGroupBox, *m_groupBox, *m_messageBox;
		QListView *m_playerList;
		QPushButton *m_connectButton;
};
