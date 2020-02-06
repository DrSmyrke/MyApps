#include "searcher.h"
//TODO: remove qDebug()
#include <QDebug>

Searcher::Searcher(QObject *parent) :
	QObject(parent)
{
	m_pSocket = new QUdpSocket(this);

	connect( m_pSocket, &QUdpSocket::readyRead, this, &Searcher::slot_readyRead );

	app::setLog(1,QString("Searcher::created"));
}

void Searcher::start()
{
	m_pSocket->bind( myproto::conf.port, QUdpSocket::ShareAddress );

	app::setLog(1,QString("Searcher::started on [%1] port").arg(myproto::conf.port));
}

void Searcher::stop()
{

}

void Searcher::slot_readyRead()
{
	QByteArray ba;

	while( m_pSocket->bytesAvailable() ) ba.append( m_pSocket->readAll() );

	qDebug()<<"Searcher::slot_readyRead"<<ba.toHex();

//	while (udpSocket->hasPendingDatagrams()) {
//	        datagram.resize(int(udpSocket->pendingDatagramSize()));
//	        udpSocket->readDatagram(datagram.data(), datagram.size());
//	        statusLabel->setText(tr("Received datagram: \"%1\"")
//	                             .arg(datagram.constData()));
//	    }
}

void Searcher::send(const QByteArray &data)
{
	//udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
}
