#include "serial.h"
//TODO: remove qdebug
#include <QDebug>

Serial::Serial(QObject *parent) :
	QObject(parent)
{
	m_pSerialPort = new QSerialPort(this);
	connect( m_pSerialPort, &QSerialPort::readyRead, this, &Serial::slot_readData );
}

void Serial::init(const SerialSettings &settings)
{
	m_pSerialPort->setPortName( settings.port );
	m_pSerialPort->setBaudRate( settings.speed );
	switch ( settings.bits ) {
		case 5: m_pSerialPort->setDataBits( QSerialPort::Data5 ); break;
		case 6: m_pSerialPort->setDataBits( QSerialPort::Data6 ); break;
		case 7: m_pSerialPort->setDataBits( QSerialPort::Data7 ); break;
		case 8: m_pSerialPort->setDataBits( QSerialPort::Data8 ); break;
		case -1: m_pSerialPort->setDataBits( QSerialPort::UnknownDataBits ); break;
		default: m_pSerialPort->setDataBits( QSerialPort::Data8 ); break;
	}
	switch ( settings.parity ) {
		case 0: m_pSerialPort->setParity( QSerialPort::NoParity ); break;
		case 2: m_pSerialPort->setParity( QSerialPort::EvenParity ); break;
		case 3: m_pSerialPort->setParity( QSerialPort::OddParity ); break;
		case 4: m_pSerialPort->setParity( QSerialPort::SpaceParity ); break;
		case 5: m_pSerialPort->setParity( QSerialPort::MarkParity ); break;
		case -1: m_pSerialPort->setParity( QSerialPort::UnknownParity ); break;
		default: m_pSerialPort->setParity( QSerialPort::NoParity ); break;
	}
	switch ( settings.stopBits ) {
		case 1: m_pSerialPort->setStopBits( QSerialPort::OneStop ); break;
		case 2: m_pSerialPort->setStopBits( QSerialPort::OneAndHalfStop ); break;
		case 3: m_pSerialPort->setStopBits( QSerialPort::TwoStop ); break;
		case -1: m_pSerialPort->setStopBits( QSerialPort::UnknownStopBits ); break;
		default: m_pSerialPort->setStopBits( QSerialPort::OneStop ); break;
	}
	switch ( settings.flow ) {
		case 0: m_pSerialPort->setFlowControl( QSerialPort::NoFlowControl ); break;
		case 1: m_pSerialPort->setFlowControl( QSerialPort::HardwareControl ); break;
		case 2: m_pSerialPort->setFlowControl( QSerialPort::SoftwareControl ); break;
		case -1: m_pSerialPort->setFlowControl( QSerialPort::UnknownFlowControl ); break;
		default: m_pSerialPort->setFlowControl( QSerialPort::NoFlowControl ); break;
	}
}

void Serial::open()
{
	if( m_pSerialPort->isOpen() ) m_pSerialPort->close();
	auto result = m_pSerialPort->open( QIODevice::ReadWrite );
	if( result ) emit signal_open();
}

void Serial::close()
{
	if( m_pSerialPort->isOpen() ) m_pSerialPort->close();
	emit signal_close();
}

void Serial::send(const QByteArray &data)
{
	if( !m_pSerialPort->isOpen() ) return;
	auto i = m_pSerialPort->write( data );
	qDebug()<<i<<data.toHex();
	//if( i == data.length() ){
	//	printf("<:%с\n", data.data() );
	//}else{
	//	printf("Serial::send ERROR %u [%s]\n", i, data.data() );
	//}
}

void Serial::slot_readData()
{
	QByteArray ba;
	while( m_pSerialPort->bytesAvailable() ) ba.append( m_pSerialPort->readAll() );
	emit signal_readData( ba );
}
