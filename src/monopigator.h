	#ifndef __KMONOP_MONOPIGATOR_H__
#define __KMONOP_MONOPIGATOR_H__

#include <qobject.h>
#include <qbuffer.h>

#include <kio/job.h>
#include <kurl.h>

class Monopigator : public QObject
{
Q_OBJECT

public:
	Monopigator();
	void loadData(const KURL &);

signals:
	void monopigatorServer(QString host, QString port);

private slots:
	void slotData(KIO::Job *, const QByteArray &);
	void slotResult(KIO::Job *);

private:
	void processData(const QByteArray &, bool = true);

	QBuffer m_downloadData;
};

#endif
