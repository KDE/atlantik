#include <qwidget.h>
#include <qlayout.h>
//#include <qvgroupbox.h>
#include <qlistview.h>
//#include <qpushbutton.h>
//#include <qtextedit.h>

class TradeDisplay : public QWidget
{
Q_OBJECT

public:
	TradeDisplay(QWidget *parent, const char *name = 0);

private slots:
	void tradeChanged();

private:
	QVBoxLayout *m_mainLayout;
	QListView *m_playerList;
};
