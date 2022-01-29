#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_working		= false;

	m_pTimer = new QTimer( this );
	m_pSPort = new QSerialPort(this);
		m_pSPort->setBaudRate( 115200 );
		m_pSPort->setDataBits( QSerialPort::DataBits::Data8 );
		m_pSPort->setParity( QSerialPort::Parity::NoParity );
		m_pSPort->setStopBits( QSerialPort::StopBits::OneStop );
		m_pSPort->setFlowControl( QSerialPort::FlowControl::NoFlowControl );

	QString version = QString("0.3.%1").arg(APP_VER_SECOND);
	setWindowTitle( "My Clock Programmer v" + version );
	setWindowIcon( QIcon( "://index.ico" ) );


	connect( ui->findB, &QPushButton::clicked, this, &MainWindow::slot_rescanPorts );
	connect( m_pTimer, &QTimer::timeout, this, [this](){
		m_curDate = QDate::currentDate();
		m_curTime = QTime::currentTime();

		auto dt = ui->dateTimeEdit->dateTime();
		if( dt.date().year() == 2000 && dt.date().month() == 1 && dt.date().day() == 1 ){
			ui->dateTimeEdit->setDate( m_curDate );
			ui->dateTimeEdit->setTime( m_curTime );
		}

		QString year	= QString::number( m_curDate.year() );
		QString month	= QString::number( m_curDate.month() );
		QString day		= QString::number( m_curDate.day() );
		QString hour	= QString::number( m_curTime.hour() );
		QString min		= QString::number( m_curTime.minute() );
		QString sec		= QString::number( m_curTime.second() );

		if( month.length() == 1 ) month.prepend( "0" );
		if( day.length() == 1 ) day.prepend( "0" );
		if( hour.length() == 1 ) hour.prepend( "0" );
		if( min.length() == 1 ) min.prepend( "0" );
		if( sec.length() == 1 ) sec.prepend( "0" );

		ui->secL->setText( sec );
		ui->minL->setText( min );
		ui->hourL->setText( hour );
		ui->dayL->setText( day );
		ui->monL->setText( month );
		ui->yearL->setText( year );
	} );

	connect( ui->moveB, &QPushButton::clicked, this, [this](){
		uint16_t year = m_curDate.year();
		QByteArray ba;
		ba.resize( 8 );
		ba[ 0 ] = (char)0xAA;
		ba[ 1 ] = (char)year >> 8;
		ba[ 2 ] = (char)year;
		ba[ 3 ] = (char)m_curDate.month();
		ba[ 4 ] = (char)m_curDate.day();
		ba[ 5 ] = (char)m_curTime.hour();
		ba[ 6 ] = (char)m_curTime.minute();
		ba[ 7 ] = (char)m_curTime.second();

		sendData( ba );
	} );

	connect( ui->setB, &QPushButton::clicked, this, [this](){
		auto dt = ui->dateTimeEdit->dateTime();
		uint16_t year = dt.date().year();
		QByteArray ba;
		ba.resize( 8 );
		ba[ 0 ] = (char)0xAA;
		ba[ 1 ] = (char)year >> 8;
		ba[ 2 ] = (char)year;
		ba[ 3 ] = (char)dt.date().month();
		ba[ 4 ] = (char)dt.date().day();
		ba[ 5 ] = (char)dt.time().hour();
		ba[ 6 ] = (char)dt.time().minute();
		ba[ 7 ] = (char)dt.time().second();

		sendData( ba );
	} );





	m_pTimer->setInterval( 500 );
	m_pTimer->start();
}

MainWindow::~MainWindow()
{
	if( m_pSPort->isOpen() ) m_pSPort->close();
	delete ui;
}

void MainWindow::slot_readyRead()
{
	while( m_pSPort->bytesAvailable() ) m_buff.append( m_pSPort->readAll() );

	if( m_buff.size() == 1 ){
		if( m_buff[ 0 ] == (char)0xAF ){
			m_buff[ 0 ] = (char)0xF1;
			sendData( m_buff );
			m_buff.clear();
			return;
		}
	}

	while( m_buff[ 0 ] != (char)0xA1 ){
		m_buff.remove( 0, 1 );
		if( m_buff.size() == 0 ) break;
	}

	if( m_buff.size() <= 0 ) return;

	if( m_buff[ 0 ] == (char)0xA1 && m_buff.size() >= 8 ){
		QString year	= QString::number( ( (uint8_t)m_buff[ 1 ] << 8 ) | ( (uint8_t)m_buff[ 2 ] ) );
		QString month	= QString::number( (uint8_t)m_buff[ 3 ] );
		QString day		= QString::number( (uint8_t)m_buff[ 4 ] );
		QString hour	= QString::number( (uint8_t)m_buff[ 5 ] );
		QString min		= QString::number( (uint8_t)m_buff[ 6 ] );
		QString sec		= QString::number( (uint8_t)m_buff[ 7 ] );



		if( month.length() == 1 ) month.prepend( "0" );
		if( day.length() == 1 ) day.prepend( "0" );
		if( hour.length() == 1 ) hour.prepend( "0" );
		if( min.length() == 1 ) min.prepend( "0" );
		if( sec.length() == 1 ) sec.prepend( "0" );


		ui->devSecL->setText( sec );
		ui->devMinL->setText( min );
		ui->devHourL->setText( hour );
		ui->devDayL->setText( day );
		ui->devMonL->setText( month );
		ui->devYearL->setText( year );

		m_buff.remove( 0, 8 );
	}

	if( m_buff.size() ) slot_readyRead();
}

void MainWindow::slot_rescanPorts()
{
	if( m_pSPort->isOpen() ){
		m_pSPort->close();
		m_buff.clear();
	}

	if( m_working ){
		disconnect( m_pSPort, &QSerialPort::readyRead, this, &MainWindow::slot_readyRead );
		m_working = false;
	}

	ui->findB->setEnabled( false );
	ui->statusL->setText( "<b>Find...</b>" );

	bool found = false;
	auto list = QSerialPortInfo::availablePorts();
	for( auto portInfo:list ){
		auto portName = portInfo.portName();
		auto pid = portInfo.hasProductIdentifier();
		auto vid = portInfo.hasVendorIdentifier();
		if( !pid || !vid ) continue;

		auto man = portInfo.manufacturer();
		auto sn = portInfo.serialNumber();
//		auto sysloc = portInfo.systemLocation();


		if( checkPort( portName, false ) ){
			found = true;
			break;
		}
	}

	if( found ){
		ui->statusL->setText( "<b>Connected</b>" );
		m_working = true;
		connect( m_pSPort, &QSerialPort::readyRead, this, &MainWindow::slot_readyRead );
	}else{
		ui->statusL->setText( "<b>Not Connected</b>" );
	}


	ui->findB->setEnabled( true );


	/*
#ifdef __linux__

#elif _WIN32
	for( uint8_t i = 1; i < 250; i++ ){
		auto str = QString("COM%1").arg( i );sendData
		if( checkPort( str ) ) ui->portBox->addItem( str );
	}
#endif
	*/
}

bool MainWindow::checkPort(const QString &port, bool close)
{
	bool res = false;

	if( m_pSPort->isOpen() ){
		if( m_pSPort->bytesAvailable() ){
			auto ba = m_pSPort->readAll();
			ba.clear();
		}
		m_pSPort->close();
	}

	m_pSPort->setPortName( port );

	if( m_pSPort->open( QIODevice::ReadWrite ) ){
		QByteArray btx;
		btx.resize( 1 );
		btx[ 0 ] = 0xF1;
		sendData( btx );
		m_pSPort->waitForReadyRead( 3000 );
		if( m_pSPort->bytesAvailable() == 1 ){
			auto ba = m_pSPort->read( 1 );
			if( ba[ 0 ] == (char)0xCF ){
				res = true;
			}else{
				close = true;
			}
		}else{
			close = true;
		}

		if( close ) m_pSPort->close();
	}

	return res;
}

void MainWindow::sendData(const QByteArray &data)
{
	if( !m_pSPort->isOpen() ){
		ui->statusL->setText( "<b>Not Connected</b>" );
		return;
	}

	if( data.size() == 0 ) return;
	m_pSPort->write( data );
	m_pSPort->waitForBytesWritten( 300 );
}


