#include <qlabel.h>

enum Orientation{ North=0, East=1, South=2, West=3 };
class EstateView : public QWidget
{
	public:
		EstateView(int _orientation, const QColor &, QWidget *parent, const char *name = 0);
		void setName(const char *);
		void setHouses(int);

	protected:
		void paintEvent(QPaintEvent *);
		void resizeEvent(QResizeEvent *);

	private:
		QPixmap *qpixmap;
		bool b_recreate;
		QColor color;		
		QLabel *lname;
		QString estatename;
		int orientation;
		int houses;
};
