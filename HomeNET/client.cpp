#include "client.h"

Client::Client(QTcpSocket *socket, const uint8_t socketType, QObject *parent)
	: QObject(parent)
	, m_pClient(socket)
	, m_socketType(socketType)
{
	connect( m_pClient, &QTcpSocket::disconnected, this, [this](){
		emit signal_disconnected( m_pClient->socketDescriptor() );
		myproto::appData.connected = false;
	} );
	connect( m_pClient, &QTcpSocket::readyRead, this, &Client::slot_readyRead );
}

Client::Client(const uint8_t socketType, QObject *parent)
	: QObject(parent)
	, m_socketType(socketType)
{
	m_pClient = new QTcpSocket(this);
	connect( m_pClient, &QTcpSocket::disconnected, this, [this](){
		emit signal_disconnected( m_pClient->socketDescriptor() );
		myproto::appData.connected = false;
	} );
	connect( m_pClient, &QTcpSocket::readyRead, this, &Client::slot_readyRead );
}

bool Client::open(const QHostAddress &addr, const uint16_t port)
{
	m_pClient->connectToHost( addr, port );
	m_pClient->waitForConnected(100);
	bool res = m_pClient->isOpen();
	if( res ) send( myproto::buidPkt( myproto::getPkt( pkt_channel_comunication, pkt_type_helo ) ) );
	app::setLog(1,QString("Client::open [%1:%2]... [%3]").arg(addr.toString()).arg(port).arg(res));
	return res;
}

bool Client::send(const QByteArray &data)
{
	if( !m_pClient->isOpen() ) return false;

	int n = m_pClient->write( data );

	qDebug()<<"Client::send"<<n<<data.toHex();

	if( n < 0 ) return false;

	return true;
}

void Client::slot_stop()
{
	emit signal_disconnected( m_pClient->socketDescriptor() );
	if( m_pClient->isOpen() ) m_pClient->close();
}

void Client::slot_readyRead()
{
	while( m_pClient->bytesAvailable() ) m_rxBuff.append( m_pClient->readAll() );

	qDebug()<<m_rxBuff.toHex();

	Pkt pkt = myproto::parsPkt( m_rxBuff );

	if( pkt.next ) return;
	if( pkt.error ){
		//TODO: resendLastPkt
		return;
	}
	if( pkt.retry ){
		slot_readyRead();
		return;
	}

	myproto::parsParams( pkt );

	switch (pkt.chanelNum) {
		case pkt_channel_comunication:		parsPktCommunication( pkt );	break;
	}
}

void Client::parsPktCommunication(const Pkt &pkt)
{
	//if( pkt.pktData )
	qDebug()<<pkt.rawData.toHex();
}
