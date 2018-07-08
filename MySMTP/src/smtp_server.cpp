#include "smtp_server.h"

SMTP_Server::SMTP_Server(QObject *parent) : QObject(parent)
{
	m_pServer = new QTcpServer(this);
	connect(m_pServer,&QTcpServer::newConnection,this,&SMTP_Server::slot_incommingConnection);
}

void SMTP_Server::start()
{
	bool res = m_pServer->listen( QHostAddress::Any, app::smtpConf.port );
	printf("SMTP SERVER at %u port %s\n", app::smtpConf.port,(res)?"STARTED":"ERROR [cannot bind to port]");
}

void SMTP_Server::stop()
{
	if(m_pServer->isListening()) m_pServer->close();
	printf("SMTP SERVER %s\n",(m_pServer->isListening())?"ERROR [cannot stop server]":"STOPPING");
}

void SMTP_Server::slot_incommingConnection()
{
	QTcpSocket* socket = m_pServer->nextPendingConnection();
	socket->write("220 mail.company.tld ESMTP is glad to see you!\r\n");
	SMTP_Client* client = new SMTP_Client( socket, this );
	connect(client,&SMTP_Client::signal_unconnected,client,&SMTP_Client::deleteLater);
}

SMTP_Client::SMTP_Client(QTcpSocket *client, QObject *parent)
	 : QObject(parent)
	 , m_pClient(client)
{
	connect(m_pClient,&QTcpSocket::stateChanged,this,&SMTP_Client::slot_stateChanged);
	connect(m_pClient,&QTcpSocket::readyRead,this,&SMTP_Client::slot_readyRead);
}

void SMTP_Client::slot_stateChanged(const QAbstractSocket::SocketState state)
{
	if( state == QAbstractSocket::UnconnectedState ) emit signal_unconnected();
}

void SMTP_Client::slot_readyRead()
{
//	QObject* object = QObject::sender();
//	if (!object) return;
//	QTcpSocket* socket = static_cast<QTcpSocket *>(object);
	QByteArray buff;
	while( m_pClient->bytesAvailable() ) buff.append( m_pClient->readAll() );

	SMTP_Pkt pkt = smtp::parsPkt( buff );
	//TODO: if( !pkt.valid )
	if( pkt.cmd == "HELO" ) m_pClient->write("250 domain name should be qualified\r\n");
	if( pkt.cmd == "EHLO" ) m_pClient->write("250 HELP\r\n");

	qDebug()<<pkt.cmd<<pkt.data;
}
