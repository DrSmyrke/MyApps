#include "server.h"

Server::Server(QObject *parent)	: QTcpServer(parent)
{
	printf("SERVER CREATING...\n");
	data::access.clear();

	if(!cfg::conf.fileAccess.isEmpty()){
		m_accessFile.setFileName(cfg::conf.fileAccess);

		if(!m_accessFile.exists()){
			if(m_accessFile.open(QIODevice::WriteOnly | QIODevice::Text)){
				m_accessFile.write("#SRC_IP	DEST_IP	DEST_PORT\n#ACCESS (*-any) (access=yes/no)\n\n");
				m_accessFile.close();
			}
		}

		if(m_accessFile.open(QIODevice::ReadOnly | QIODevice::Text)){
			while(!m_accessFile.atEnd()){
				auto data=m_accessFile.readLine().replace("\n","");
				if(data.indexOf("#")==0) continue;
				data::access.push_back(data);
			}
			m_accessFile.close();
		}
	}
}

Server::~Server()
{
	emit signal_stopAll();
}

void Server::run()
{
	if(!this->listen(QHostAddress::Any,cfg::conf.port)){
		qWarning("ERROR\n");
	}else{
		printf("SERVER [ ACTIVATED ] PORT: [%i]\n",cfg::conf.port);
	}
}

void Server::incomingConnection(qintptr socketDescriptor)
{
	Client* client=new Client(socketDescriptor);
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
