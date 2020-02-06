#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "myproto.h"
#include "global.h"
#include "myfunctions.h"

class Client : public QObject
{
	Q_OBJECT
public:
	explicit Client( QTcpSocket* socket, const uint8_t socketType, QObject *parent = nullptr);
	explicit Client( const uint8_t socketType, QObject *parent = nullptr);
	int getSocketDescriptor(){ return m_pClient->socketDescriptor(); }
	//ClientData getData(){ return m_data; }
	bool open(const QHostAddress &addr , const uint16_t port);
	bool send( const QByteArray &data );
signals:
	void signal_disconnected( const int sd );
public slots:
	void slot_stop();
private slots:
	void slot_readyRead();
private:
	QTcpSocket* m_pClient;
	QByteArray m_rxBuff;
	//ClientData m_data;
	uint8_t m_socketType;

	void parsPktCommunication( const Pkt &pkt );
};

#endif // CLIENT_H
