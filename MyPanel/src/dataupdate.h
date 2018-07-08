#ifndef DATAUPDATE_H
#define DATAUPDATE_H

#include <QObject>
#include <QTimer>
#include "global.h"
#include "wget.h"

enum{
	wget_operation_getKotirovki = 1,
};

class DataUpdate : public QObject
{
	Q_OBJECT
public:
	explicit DataUpdate(QObject *parent = 0);
signals:
	void signal_finished();
	void signal_applicationUpdate();
public slots:
	void slot_stop();
	void updateValuta();
private:
	Wget* m_pWget;
	QTimer* m_pTimer;
	uint8_t m_wgetOperation;

	void parsingValuta(const QString &data);
};

#endif // DATAUPDATE_H
