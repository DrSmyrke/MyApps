#include "mainwindow.h"
#include <QLayout>
//TODO: remove qdebug
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	m_pSerial = new Serial(this);

	QWidget* centrWidget = new QWidget(this);
//		QHBoxLayout* hBox = new QHBoxLayout();
//			hBox->setSpacing(3);
//			hBox->setMargin(1);
//		hBox->addWidget(m_pMenuB);
//		hBox->addWidget(trashB);
//		hBox->addWidget(winKillerB);
//		hBox->addWidget(rsyncB);
//	centrWidget->setLayout(hBox);

	setCentralWidget(centrWidget);
	setWindowTitle( "My Serial Terminal" );
	this->activateWindow();

	connect( m_pSerial, &Serial::signal_readData, this, []( const QByteArray &rxData ){
		//printf(">:%с\n", rxData.data() );
		qDebug()<<rxData.toHex();
	} );
}

MainWindow::~MainWindow()
{

}

void MainWindow::run()
{
	app::conf.serial.port = "/dev/ttyUSB0";

	m_pSerial->init( app::conf.serial );
	m_pSerial->open();
	QByteArray ba;
	ba[0] = 0xaa;
	ba[1] = 0xab;
	ba[2] = 0x00;
	ba[3] = 0xab;

	ba[4] = 0xaa;
	ba[5] = 0xae;
	ba[6] = 0x00;
	ba[7] = 0xae;

	ba[8] = 0xaa;
	ba[9] = 0xb0;
	ba[10] = 0x00;
	ba[11] = 0xb0;

	m_pSerial->send( ba );
}
