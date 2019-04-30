#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
	m_Server = new QTcpServer(this);
	connect( m_Server, &QTcpServer::newConnection, this, &Server::slot_incommingConnection );
	mf::setLog(1,QString("HomeNET Server created"));
}

bool Server::start()
{
	m_clients.clear();
	bool res = m_Server->listen( QHostAddress::Any, myproto::conf.port );
	mf::setLog(1,QString("HomeNET Server::start at %1 port ... [%2]").arg(myproto::conf.port).arg(res));
	return res;
}

void Server::stop()
{
	mf::setLog(1,QString("HomeNET Server stopped"));
	emit signal_finished();
	m_Server->close();
}

void Server::slot_incommingConnection()
{
	mf::setLog(1,QString("HomeNET Server incommingConnection"));
	auto  socket = m_Server->nextPendingConnection();

	Client* client = new Client( socket, socket_type_server, this );

	m_clients.push_back( client );

	connect( this, &Server::signal_finished, client, &Client::slot_stop );
	connect( client, &Client::signal_disconnected, this, [this]( const int sd ){
		for( auto client:m_clients ){
			if( client->getSocketDescriptor() == sd ){
				m_clients.remove( client );
				client->deleteLater();
				break;
			}
		}
	} );
}
