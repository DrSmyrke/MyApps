#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include "myproto.h"
#include "client.h"

class Server : public QObject
{
	Q_OBJECT
public:
	explicit Server(QObject *parent = nullptr);
	bool start();
public slots:
	void stop();
signals:
	void signal_finished();
private slots:
	void slot_incommingConnection();
private:
	QTcpServer* m_Server;
	std::list<Client*> m_clients;
};

#endif // SERVER_H
