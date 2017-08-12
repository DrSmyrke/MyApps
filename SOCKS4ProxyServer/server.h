#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include "client.h"

#include "global.h"

class Server : public QTcpServer
{
	Q_OBJECT
public:
	explicit Server(QObject *parent = 0);
	~Server();
	void run();
protected:
	void incomingConnection(qintptr socketDescriptor);
signals:
	void signal_stopAll();
private:
	QFile m_accessFile;
};

#endif // SERVER_H
