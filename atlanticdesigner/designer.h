#ifndef DESIGNER_H
#define DESIGNER_H

#include <kmainwindow.h>
#include <qstring.h>

class EstateEdit;
class QCloseEvent;

class TopLevel : public KMainWindow
{
	Q_OBJECT

	public:
	TopLevel(QWidget *parent = 0, const char *name = 0);

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void open();
	void openNew();
	void save();

	private:
	EstateEdit *editor;
};

#endif
