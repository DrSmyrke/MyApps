#ifndef SMTP_SERVER_H
#define SMTP_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "global.h"

class SMTP_Client : public QObject
{
	Q_OBJECT
public:
	explicit SMTP_Client(QTcpSocket* client, QObject *parent = 0);
signals:
	void signal_unconnected();
private slots:
	void slot_stateChanged(const QAbstractSocket::SocketState state);
	void slot_readyRead();
private:
	QTcpSocket* m_pClient;
	bool m_inData = false;
	Mail m_tmpMail;

	void stop();
};

class SMTP_Server : public QObject
{
	Q_OBJECT
public:
	explicit SMTP_Server(QObject *parent = 0);
	bool start();
	void stop();
private slots:
	void slot_incommingConnection();
private:
	QTcpServer* m_pServer;
};

#endif // SMTP_SERVER_H
