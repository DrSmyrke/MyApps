#include "client.h"
#include <QHostAddress>
#include <QDateTime>
#include <QDir>

Client::Client(qintptr descriptor, QObject *parent)
	: QThread(parent)
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
		if(!m_cmd){
			auto clientData=m_pClient->readAll();
			//qDebug()<<"CLIENT:"<<data.toHex();
			//int cmd=data[1];
			QByteArray rport;
				rport.append(clientData[2]);
				rport.append(clientData[3]);
			//int tmp=rport.toHex().toInt(nullptr,16);
			//qDebug()<<cmd<<rport.toHex()<<tmp;
			QByteArray ip1;
				ip1.append(clientData[4]);
			QByteArray ip2;
				ip2.append(clientData[5]);
			QByteArray ip3;
				ip3.append(clientData[6]);
			QByteArray ip4;
				ip4.append(clientData[7]);
			//qDebug()<<cmd<<rport.toHex()<<ip1.toHex()<<ip2.toHex()<<ip3.toHex()<<ip4.toHex();
			QString ip=QString::number(ip1.toHex().toInt(nullptr,16))
					+"."+QString::number(ip2.toHex().toInt(nullptr,16))
					+"."+QString::number(ip3.toHex().toInt(nullptr,16))
					+"."+QString::number(ip4.toHex().toInt(nullptr,16));

			QString time=QDateTime::currentDateTime().toString("[hh:mm:ss]");
			QHostAddress clientAddr;
				clientAddr.setAddress(m_pClient->peerAddress().toIPv4Address());
			QString logStr=time+" "+clientAddr.toString()+" -> "+ip+":"+QString::number(rport.toHex().toInt(nullptr,16));

			if(cfg::conf.verbose) printf("%s ... ",logStr.toStdString().c_str());

			//access
			bool accessFlag=true;
			for(auto str:data::access){
				auto list=str.split("	");
				if(list.size()!=4) continue;
				if(list[3]=="yes"){
					if(list[0]=="*" or list[0]==clientAddr.toString()){
						if(list[1]==ip or list[1]=="*"){
							if(list[2].toInt()==rport.toHex().toInt(nullptr,16) or list[2]=="*"){
								accessFlag=true;
								continue;
							}
						}
					}
				}
				if(list[3]=="no"){
					if(list[0]=="*" or list[0]==clientAddr.toString()){
						if(list[1]==ip or list[1]=="*"){
							if(list[2].toInt()==rport.toHex().toInt(nullptr,16) or list[2]=="*"){
								accessFlag=false;
								continue;
							}
						}
					}
				}
			}

			if(accessFlag){
				m_pRClient->connectToHost(ip,rport.toHex().toInt(nullptr,16));
				m_pRClient->waitForConnected(3000);
				if(m_pRClient->isOpen()){
					QByteArray ret;
					ret[0]=0x00;
					ret[1]=0x5a;
					ret[2]=0x00;
					ret[3]=0x00;
					ret[4]=0x00;
					ret[5]=0x00;
					ret[6]=0x00;
					ret[7]=0x00;
					m_pClient->write(ret,ret.size());
					m_cmd=true;
					if(cfg::conf.verbose) printf("OK\n");
					if(cfg::conf.logging) log(logStr+" OK");
				}else{
					if(cfg::conf.verbose) printf("ERROR\n");
					if(cfg::conf.logging) log(m_pClient->localAddress().toString()+"->"+ip+":"+QString::number(rport.toHex().toInt(nullptr,16))+" ERROR");
					sendError();
				}
			}else{
				if(cfg::conf.verbose) printf("DENIED\n");
				if(cfg::conf.logging) log(m_pClient->localAddress().toString()+"->"+ip+":"+QString::number(rport.toHex().toInt(nullptr,16))+" DENIED");
				sendError();
			}
		}else{
			if(!m_pRClient->isOpen()) m_pClient->close();
			m_pRClient->write(m_pClient->readAll());
			m_pRClient->waitForBytesWritten(3000);
		}
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
	//m_pRClient->connectToHost(m_dist_addr,m_dist_port);
	//m_pRClient->waitForConnected(3000);
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
//				out.push_back("Host: "+m_dist_addr+":"+QString::number(m_dist_port));
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

void Client::sendError()
{
	QByteArray ret;
		ret[0]=0x00;
		ret[1]=0x5b;
		ret[2]=0x00;
		ret[3]=0x00;
		ret[4]=0x00;
		ret[5]=0x00;
		ret[6]=0x00;
		ret[7]=0x00;
	m_pClient->write(ret,ret.size());
	slot_stop();
}

void Client::log(const QString &text)
{
	if(!cfg::conf.logging or text.isEmpty()) return;
	QString logFile=QDateTime::currentDateTime().toString("yyyy-MM-dd.log");
	if(!QDir(cfg::conf.logsPath).exists()) QDir().mkpath(cfg::conf.logsPath);
	if(!QDir(cfg::conf.logsPath).exists()){
		printf("logs path [%s] not found\n",cfg::conf.logsPath.toStdString().c_str());
	}
	QFile file(cfg::conf.logsPath+"/"+logFile);
	if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
		file.write(QString(text+"\n").toStdString().c_str());
		file.close();
	}
}
