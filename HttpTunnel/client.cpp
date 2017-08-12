#include "client.h"

Client::Client(qintptr descriptor, const QString &dest_addr, const int dest_port, QObject *parent)
	: QThread(parent)
	, m_dist_port(dest_port)
	, m_dist_addr(dest_addr)
{
	m_pClient=new QTcpSocket();
	if(!m_pClient->setSocketDescriptor(descriptor)) emit signal_finished();
	m_pRClient=new QTcpSocket();

	connect(m_pRClient,&QTcpSocket::readyRead,this,[this](){
		if(!m_pRClient->isReadable()) m_pRClient->waitForReadyRead(3000);
		m_pClient->write(m_pRClient->readAll());
		m_pClient->waitForBytesWritten(3000);
	});
	connect(m_pClient,&QTcpSocket::readyRead,this,[this](){
		if(!m_pClient->isReadable()) m_pClient->waitForReadyRead(3000);
		QString data=parsHead(QString(m_pClient->readAll()));
		//qDebug()<<"CLIENT:"<<data;
		if(!m_pRClient->isOpen()) slot_start();
		m_pRClient->write(data.toStdString().c_str());
		m_pRClient->waitForBytesWritten(3000);
	});

	connect(m_pClient,&QTcpSocket::disconnected,this,[this](){
		if(m_pRClient->isOpen()) m_pRClient->close();
		emit signal_finished();
	});
	connect(m_pRClient,&QTcpSocket::disconnected,this,[this](){
		if(m_pClient->isOpen()) m_pClient->close();
		emit signal_finished();
	});
}

void Client::slot_start()
{
	m_pRClient->connectToHost(m_dist_addr,m_dist_port);
	m_pRClient->waitForConnected(3000);
}

void Client::slot_stop()
{
	if(m_pRClient->isOpen()) m_pRClient->close();
	if(m_pClient->isOpen()) m_pClient->close();
	emit signal_finished();
}

QString &Client::parsHead(const QString &data)
{
	QStringList out;
	static QString outStr;
	if(data.indexOf("GET /",Qt::CaseInsensitive)==0 or data.indexOf("POST /",Qt::CaseInsensitive)==0){
		for(auto str:data.split("\n")){
			if(str.indexOf("Host: ",Qt::CaseInsensitive)==0){
				out.push_back("Host: "+m_dist_addr+":"+QString::number(m_dist_port));
				continue;
			}
			out.push_back(str);
		}
	}else{
		outStr=data;
		return outStr;
	}
	outStr=out.join("\n");
	return outStr;
}
