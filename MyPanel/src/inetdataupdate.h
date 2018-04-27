#ifndef INETDATAUPDATE_H
#define INETDATAUPDATE_H

#include <QObject>
#include <QTimer>
#include "global.h"
#include "wget.h"

enum{
	wget_operation_getKotirovki = 1,
};

class InetDataUpdate : public QObject
{
	Q_OBJECT
public:
	explicit InetDataUpdate(QObject *parent = 0);
	void updateAll();
	void updateValuta();
private:
	Wget* m_pWget;
	QTimer* m_pTimer;
	uint8_t m_wgetOperation;
	bool m_updateAllF = false;

	void parsingValuta(const QString &data);
};

#endif // INETDATAUPDATE_H
