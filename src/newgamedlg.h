#include <qlistview.h>
#include <qpushbutton.h>

class NewGameDialog : public QWidget
{
Q_OBJECT

	public:
		NewGameDialog(QWidget *parent, const char *name = 0);
	public slots:
		void slotConnect();
		void slotCancel();
	protected:
		void paintEvent(QPaintEvent *);
	private:
		QListView *list;
		QPushButton *bconnect, *bcancel;
};
