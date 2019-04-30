#include "smtp_server.h"

SMTP_Server::SMTP_Server(QObject *parent) : QObject(parent)
{
	m_pServer = new QTcpServer(this);
	connect(m_pServer,&QTcpServer::newConnection,this,&SMTP_Server::slot_incommingConnection);
}

bool SMTP_Server::start()
{
	bool res = m_pServer->listen( QHostAddress::Any, app::conf.port );
	app::setLog(1,QString("SMTP_Server::start at [%1] port %2").arg(app::conf.port).arg((res)?"STARTED":"ERROR [cannot bind to port]"));
	return res;
}

void SMTP_Server::stop()
{
	if(m_pServer->isListening()) m_pServer->close();
	app::setLog(1,QString("SMTP_Server::stop %1").arg((m_pServer->isListening())?"ERROR [cannot stop server]":"STOPPING"));
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
	QByteArray buff;
	while( m_pClient->bytesAvailable() ) buff.append( m_pClient->readAll() );

	if(m_inData){
		m_tmpMail.message += buff;
		if(m_tmpMail.message.right(5) == "\r\n.\r\n"){
			m_inData = false;
			m_pClient->write("250 message accepted for delivery\r\n");
			m_tmpMail.theme = smtp::getTheme( m_tmpMail.message );
			app::setLog(1,QString("SMTP_Server::slot_readyRead m_tmpMail.theme [%1]").arg(m_tmpMail.theme));
			//m_tmpMail.messageNew = smtp::parsBody( m_tmpMail.message );
			app::saveNewMess( m_tmpMail );
		}
		return;
	}

	SMTP_Pkt pkt = smtp::parsPkt( buff );
	//TODO: if( !pkt.valid )

	if( pkt.cmd == "HELO" || (pkt.cmd == "" && pkt.data == "HELO") ){
		m_pClient->write("250 domain name should be qualified\r\n");
		m_tmpMail.to.clear();
		return;
	}
	if( pkt.cmd == "EHLO" || (pkt.cmd == "" && pkt.data == "EHLO") ){
		m_pClient->write("250 HELP\r\n");
		m_tmpMail.to.clear();
		return;
	}
	if( pkt.cmd == "MAILFROM" ){
		QString email = smtp::parsEmail( pkt.data );
		if( app::isBlack( email ) ){
			m_pClient->write(("251 " + email + " sender is bad :)\r\n").toStdString().c_str());
			stop();
		}else{
			m_pClient->write(("250 " + email + " sender accepted\r\n").toStdString().c_str());
			m_tmpMail.from = email;
		}
		return;
	}
	if( pkt.cmd == "RCPTTO" ){
		QString email = smtp::parsEmail( pkt.data );
		//if( app::isBlack( email ) ){
		//	m_pClient->write(("550 " + email + " unknown user account\r\n").toStdString().c_str());
		//}else{
			m_pClient->write(("250 " + email + " ok\r\n").toStdString().c_str());
			m_tmpMail.to.push_back( email );
		//}
		return;
	}
	if( pkt.cmd == "" && pkt.data == "DATA" ){
		m_inData = true;
		m_pClient->write("354 Enter mail, end with \".\" on a line by itself\r\n");
		return;
	}
	if( pkt.cmd == "" && pkt.data == "QUIT" ){
		stop();
		return;
	}

	qDebug()<<pkt.cmd<<pkt.data;
}

void SMTP_Client::stop()
{
	m_pClient->write("221 SMTP closing connection\r\n");
	m_pClient->waitForBytesWritten(3000);
	m_pClient->close();
	emit signal_unconnected();
}
