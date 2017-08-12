#include "server.h"
#include <QDebug>

Server::Server(const int local_port, const int dest_port, const QString &dest_addr, QObject *parent)
	: QTcpServer(parent)
	, m_local_port(local_port)
	, m_dist_port(dest_port)
	, m_dist_addr(dest_addr)
{
	printf("SERVER CREATING...\n");
}

void Server::start()
{
	if(!this->listen(QHostAddress::Any,m_local_port)){
		qWarning("ERROR\n");
	}else{
		printf("SERVER [ ACTIVATED ] PORT: [%i]\n",m_local_port);
	}
}

void Server::incomingConnection(qintptr socketDescriptor)
{
	Client* client=new Client(socketDescriptor,m_dist_addr,m_dist_port);
	//QThread* thread = new QThread();
	//client->moveToThread(thread);
//	connect(thread,&QThread::started,client,&Client::slot_start);
//	connect(client,&Client::signal_finished,thread,&QThread::quit);
	connect(this,&Server::signal_stopAll,client,&Client::slot_stop);
	connect(client,&Client::signal_finished,client,&Client::deleteLater);
//	connect(thread,&QThread::finished,thread,&QThread::deleteLater);
//	thread->start();
	client->run();
}
