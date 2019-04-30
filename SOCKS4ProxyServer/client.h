#ifndef CLIENT_H
#define CLIENT_H

#include <QThread>
#include <QTcpSocket>

#include "global.h"

class Client : public QThread
{
	Q_OBJECT
public:
	explicit Client(qintptr descriptor, QObject *parent = 0);
	void run() { slot_start(); }
signals:
	void signal_finished();
public slots:
	void slot_start();
	void slot_stop();
private:
	QTcpSocket* m_pClient;
	QTcpSocket* m_pRClient;
	bool m_cmd=false;

	QString& parsHead(const QString &data);
	void sendError();
	void log(const QString &text);
};

#endif // CLIENT_H
