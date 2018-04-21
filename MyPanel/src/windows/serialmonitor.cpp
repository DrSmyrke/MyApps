#include "serialmonitor.h"
#include <QLayout>
#include <QLabel>
#include <QScrollBar>

SerialMonitor::SerialMonitor(QWidget *parent) : QMainWindow(parent)
{
	m_pSerialPort=new QSerialPort(this);
		connect(m_pSerialPort,&QSerialPort::readyRead,this,&SerialMonitor::slot_serialPortReadData);
	m_pTimer=new QTimer();
		m_pTimer->setInterval(250);
		connect(m_pTimer,&QTimer::timeout,this,[this](){
			if(m_pSerialPort->isOpen()) return;
			if(m_pSerialPort->bytesAvailable()) slot_serialPortReadData();
		});
		m_pTimer->start();
	m_pConsole = new QTextBrowser(this);
		m_pConsole->setReadOnly(true);
	QPushButton* modeB = new QPushButton(tr("HEX"),this);
		modeB->setCheckable(true);
	QPushButton* clearB = new QPushButton(tr("CLEAR"),this);
	QPushButton* settingsB = new QPushButton(QIcon("://img/system.png"),"",this);
		settingsB->setCheckable(true);

	m_pPort = new QComboBox(this);
	m_pSpeed = new QComboBox(this);
		m_pSpeed->addItems(QStringList()<<"9600"<<"115200");
	m_pParity = new QComboBox(this);
		m_pParity->addItem(tr("No Parity"),QVariant(0));
		m_pParity->addItem(tr("Even Parity"),QVariant(2));
		m_pParity->addItem(tr("Odd Parity"),QVariant(3));
		m_pParity->addItem(tr("Space Parity"),QVariant(4));
		m_pParity->addItem(tr("Mark Parity"),QVariant(5));
		m_pParity->addItem(tr("Unknown Parity"),QVariant(-1));
	m_pDataBits = new QComboBox(this);
		m_pDataBits->addItem("5",QVariant(5));
		m_pDataBits->addItem("6",QVariant(6));
		m_pDataBits->addItem("7",QVariant(7));
		m_pDataBits->addItem("8",QVariant(8));
		m_pDataBits->addItem(tr("Unknown"),QVariant(-1));
	m_pStopBits = new QComboBox(this);
		m_pStopBits->addItem(tr("One Stop"),QVariant(1));
		m_pStopBits->addItem(tr("One And Half Stop"),QVariant(3));
		m_pStopBits->addItem(tr("Two Stop"),QVariant(2));
		m_pStopBits->addItem(tr("Unknown Stop Bits"),QVariant(-1));
	m_pFolowControl = new QComboBox(this);
		m_pFolowControl->addItem(tr("No Flow Control"),QVariant(0));
		m_pFolowControl->addItem(tr("Hardware Control"),QVariant(1));
		m_pFolowControl->addItem(tr("Software Control"),QVariant(2));
		m_pFolowControl->addItem(tr("Unknown Flow Control"),QVariant(-1));
	m_pResolution = new QComboBox(this);
		m_pResolution->addItems(QStringList()<<"320x240"<<"480x320"<<"640x480");



	m_pConfWidget = new QWidget(this);
		QGridLayout* confBox = new QGridLayout();
			confBox->addWidget(new QLabel(tr("Port:")),0,0);
			confBox->addWidget(m_pPort,0,1);
			confBox->addWidget(new QLabel(tr("Speed:")),1,0);
			confBox->addWidget(m_pSpeed,1,1);
			confBox->addWidget(new QLabel(tr("DataBits:")),2,0);
			confBox->addWidget(m_pDataBits,2,1);
			confBox->addWidget(new QLabel(tr("Parity:")),3,0);
			confBox->addWidget(m_pParity,3,1);
			confBox->addWidget(new QLabel(tr("StopBits:")),4,0);
			confBox->addWidget(m_pStopBits,4,1);
			confBox->addWidget(new QLabel(tr("FolowControl:")),5,0);
			confBox->addWidget(m_pFolowControl,5,1);
			confBox->addWidget(new QLabel(tr("Resolution:")),6,0);
			confBox->addWidget(m_pResolution,6,1);
		m_pConfWidget->hide();
		m_pConfWidget->setLayout(confBox);



	QWidget* centrWidget = new QWidget(this);
		QVBoxLayout* vBox = new QVBoxLayout();
//			hBox->setSpacing(3);
//			hBox->setMargin(1);
		vBox->addWidget(m_pConsole);
		vBox->addWidget(m_pConfWidget);
		QHBoxLayout* hBox = new QHBoxLayout();
		hBox->addWidget(modeB);
		hBox->addWidget(clearB);
		hBox->addWidget(settingsB);
		vBox->addLayout(hBox);
	centrWidget->setLayout(vBox);

	setCentralWidget(centrWidget);
	setWindowFlags(Qt::Popup);
	setFixedSize(320,240);

	connect(clearB,&QPushButton::clicked,this,[this](){
		m_pConsole->clear();
		m_colCounter = 0;
		m_symCounter = 0;
	});

	connect(modeB,&QPushButton::clicked,this,[this](bool checked){
		m_hexmode = checked;
		if(checked){
			QByteArray tmp = m_pConsole->toPlainText().toUtf8();
			m_pConsole->clear();
			m_colCounter = 0;
			m_symCounter = 0;
			printHex(tmp);
		}else{
			QByteArray tmp = m_pConsole->toPlainText().toUtf8();
			m_pConsole->clear();
			m_colCounter = 0;
			m_symCounter = 0;
			m_pConsole->insertPlainText( printSym(tmp) );
			m_pConsole->verticalScrollBar()->setValue( m_pConsole->verticalScrollBar()->maximum() );
		}
	});
	connect(settingsB,&QPushButton::clicked,this,[this](bool checked){
		if(checked){
			m_pConsole->hide();
			reScanComPorts();
			m_pConfWidget->show();
		}else{
			m_pConfWidget->hide();
			m_pConsole->show();
			if(m_changeSettings){
				app::saveSettings();
				connecting();
				m_changeSettings = false;
			}
		}
	});
	connect(m_pPort,&QComboBox::currentTextChanged,this,[this](const QString &text){
		app::conf.serialPort.port = text;
		m_changeSettings = true;
	});
	connect(m_pSpeed,&QComboBox::currentTextChanged,this,[this](const QString &text){
		app::conf.serialPort.speed = text.toUInt();
		m_changeSettings = true;
	});
	connect(m_pParity,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,[this](const int index){
		app::conf.serialPort.parity = m_pParity->itemData(index).toInt();
		m_changeSettings = true;
	});
	connect(m_pDataBits,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,[this](const int index){
		app::conf.serialPort.dataBits = m_pDataBits->itemData(index).toInt();
		m_changeSettings = true;
	});
	connect(m_pStopBits,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,[this](const int index){
		app::conf.serialPort.stopBits = m_pStopBits->itemData(index).toInt();
		m_changeSettings = true;
	});
	connect(m_pFolowControl,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,[this](const int index){
		app::conf.serialPort.folowControl = m_pFolowControl->itemData(index).toInt();
		m_changeSettings = true;
	});
	connect(m_pResolution,&QComboBox::currentTextChanged,this,[this](const QString &text){
		app::conf.serialMonitor = text;
		m_changeSettings = true;
		resizeMonitor();
	});

	resizeMonitor();
}

void SerialMonitor::slot_serialPortReadData()
{
	m_pSerialPort->waitForReadyRead(100);
	while(m_pSerialPort->bytesAvailable()){
		QByteArray data = m_pSerialPort->read(1);

		if(!m_hexmode){
			m_pConsole->insertPlainText(data);
			m_pConsole->verticalScrollBar()->setValue( m_pConsole->verticalScrollBar()->maximum() );
		}else{
			printHex( data );
		}
	}
}

void SerialMonitor::open()
{
	if( !this->isHidden() ) return;
	this->show();
	if(m_pSerialPort->isOpen()) m_pSerialPort->close();

	// Load settings
	m_pResolution->setCurrentText( app::conf.serialMonitor );
	m_pPort->setCurrentText( app::conf.serialPort.port );
	m_pSpeed->setCurrentText( QString::number(app::conf.serialPort.speed) );
	for(uint16_t i = 0; i < m_pParity->count(); i++){
		if( m_pParity->itemData(i).toInt() == app::conf.serialPort.parity ){
			m_pParity->setCurrentText( m_pParity->itemText(i) );
			break;
		}
	}
	for(uint16_t i = 0; i < m_pDataBits->count(); i++){
		if( m_pDataBits->itemData(i).toInt() == app::conf.serialPort.dataBits ){
			m_pDataBits->setCurrentText( m_pDataBits->itemText(i) );
			break;
		}
	}
	for(uint16_t i = 0; i < m_pStopBits->count(); i++){
		if( m_pStopBits->itemData(i).toInt() == app::conf.serialPort.stopBits ){
			m_pStopBits->setCurrentText( m_pStopBits->itemText(i) );
			break;
		}
	}
	for(uint16_t i = 0; i < m_pFolowControl->count(); i++){
		if( m_pFolowControl->itemData(i).toInt() == app::conf.serialPort.folowControl ){
			m_pFolowControl->setCurrentText( m_pFolowControl->itemText(i) );
			break;
		}
	}

	connecting();
}

void SerialMonitor::close()
{
	if( this->isHidden() ) return;
	this->hide();
	if(m_pSerialPort->isOpen()) m_pSerialPort->close();
}

void SerialMonitor::reScanComPorts()
{
	m_pPort->clear();
	QStringList devs;
	#if defined(Q_OS_WIN)
		//m_path.replace("file:///","");
		return;
	#elif defined(Q_OS_UNIX)
		QDir dir=QDir("/dev");
		devs=dir.entryList(QStringList() << "ttyUSB*",QDir::NoDotAndDotDot | QDir::System);
		m_pPort->addItems(devs);
		devs=dir.entryList(QStringList() << "ttyACM*",QDir::NoDotAndDotDot | QDir::System);
		m_pPort->addItems(devs);
#endif
}

QString SerialMonitor::printSym(const QString &data)
{
	QString str;
	QByteArray buff;
	for(unsigned int i = 0; i < data.size(); i++){
		if(data[i] == ' ' or data[i] == '	' or data[i] == '\n') continue;
		buff.append( data[i] );
		if( buff.size() == 2 ){
			QByteArray ba;
			ba.append( buff.toUInt(nullptr,16) );
			str += ba;
			buff.clear();
		}
	}
	return str;
}

void SerialMonitor::connecting()
{
	if(m_pSerialPort->isOpen()) m_pSerialPort->close();
	if( app::conf.serialPort.port.isEmpty() ) return;

	m_pSerialPort->setPortName(app::conf.serialPort.port);
	m_pSerialPort->setBaudRate(app::conf.serialPort.speed);
	switch (app::conf.serialPort.dataBits) {
		case 5: m_pSerialPort->setDataBits(QSerialPort::Data5); break;
		case 6: m_pSerialPort->setDataBits(QSerialPort::Data6); break;
		case 7: m_pSerialPort->setDataBits(QSerialPort::Data7); break;
		case 8: m_pSerialPort->setDataBits(QSerialPort::Data8); break;
		case -1: m_pSerialPort->setDataBits(QSerialPort::UnknownDataBits); break;
		default: m_pSerialPort->setDataBits(QSerialPort::Data8); break;
	}
	switch (app::conf.serialPort.parity) {
		case 0: m_pSerialPort->setParity(QSerialPort::NoParity); break;
		case 2: m_pSerialPort->setParity(QSerialPort::EvenParity); break;
		case 3: m_pSerialPort->setParity(QSerialPort::OddParity); break;
		case 4: m_pSerialPort->setParity(QSerialPort::SpaceParity); break;
		case 5: m_pSerialPort->setParity(QSerialPort::MarkParity); break;
		case -1: m_pSerialPort->setParity(QSerialPort::UnknownParity); break;
		default: m_pSerialPort->setParity(QSerialPort::NoParity); break;
	}
	switch (app::conf.serialPort.stopBits) {
		case 1: m_pSerialPort->setStopBits(QSerialPort::OneStop); break;
		case 3: m_pSerialPort->setStopBits(QSerialPort::OneAndHalfStop); break;
		case 2: m_pSerialPort->setStopBits(QSerialPort::TwoStop); break;
		case -1: m_pSerialPort->setStopBits(QSerialPort::UnknownStopBits); break;
		default: m_pSerialPort->setStopBits(QSerialPort::OneStop); break;
	}
	switch (app::conf.serialPort.stopBits) {
		case 0: m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl); break;
		case 1: m_pSerialPort->setFlowControl(QSerialPort::HardwareControl); break;
		case 2: m_pSerialPort->setFlowControl(QSerialPort::SoftwareControl); break;
		case -1: m_pSerialPort->setFlowControl(QSerialPort::UnknownFlowControl); break;
		default: m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl); break;
	}
	m_pSerialPort->open(QIODevice::ReadWrite);
}

void SerialMonitor::resizeMonitor()
{
	auto tmp = app::conf.serialMonitor.split("x");
	if( tmp.size() != 2 ) return;
	this->setFixedSize( tmp[0].toUInt(), tmp[1].toUInt() );
}

bool SerialMonitor::chkRow(const uint8_t val)
{
	bool res = false;
	switch (this->width()) {
		case 320: if(val == 3) res = true; break;
		case 480: if(val == 5) res = true; break;
		case 640: if(val == 7) res = true; break;
	}
	return res;
}

void SerialMonitor::printHex(QByteArray &data)
{
	while( data.size() > 0 ){
		QByteArray buff;
		buff.append(data.left(1));
		data.remove(0,1);
		m_symCounter++;
		m_pConsole->insertPlainText( buff.toHex() + " " );
		if( m_symCounter == 4 ){
			m_pConsole->insertPlainText( "	" );
			m_colCounter++;
			m_symCounter = 0;
		}
		if( chkRow(m_colCounter) ){
			m_pConsole->insertPlainText( "\n" );
			m_colCounter = 0;
		}
	}
	m_pConsole->verticalScrollBar()->setValue( m_pConsole->verticalScrollBar()->maximum() );
}

