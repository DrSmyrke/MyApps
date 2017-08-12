#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QTcpSocket>
#include "client.h"

class Server : public QTcpServer
{
	Q_OBJECT
public:
	explicit Server(const int local_port, const int dest_port, const QString &dest_addr, QObject *parent = 0);
	void start();
protected:
	void incomingConnection(qintptr socketDescriptor);
signals:
	void signal_stopAll();
private:
	int m_local_port;
	int m_dist_port;
	QString m_dist_addr;
};

#endif // SERVER_H
