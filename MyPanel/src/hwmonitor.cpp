#include "hwmonitor.h"
#include <QDateTime>
#include <QProcess>
#include <QDir>
#include "Net/netif.h"
//TODO: remove qDebug
#include <QDebug>

HWMonitor::HWMonitor(QObject *parent) : QObject(parent)
{
	m_pTimer = new QTimer(this);
		m_pTimer->setInterval(250);

		connect(m_pTimer,&QTimer::timeout,this,&HWMonitor::slot_timer);
}

void HWMonitor::start()
{
	m_pTimer->start();
}

void HWMonitor::stop()
{
	m_pTimer->stop();
}

void HWMonitor::slot_timer()
{
	static int8_t counter = 0;

	QDateTime dt = QDateTime::currentDateTime();
	m_data.time = dt.toString("HH:mm:ss");
	m_data.date = dt.toString("ddd dd.MM.yyyy");

	if(counter == 1){
		getUptime();
		getProcess();
	}
	if(counter == 2){
		getCpu();
		getMem();
	}
	if(counter == 3){
		getIfaces();
	}

	if(counter == 4){
		getDevs();
		counter = 0;
	}
	counter++;
}

void HWMonitor::getUptime()
{
	FILE* f = fopen("/proc/uptime","r");
	QByteArray buff;
	char ch;
	uint8_t n;
	while( (n = fread(&ch,1,1,f) ) > 0 ){
		if(ch == ' ' or ch == '	') break;
		buff.append(ch);
	}
	fclose(f);

	float ut = buff.toFloat();
	uint8_t h = ut / 60.0 / 60.0;
	uint8_t m = ( (ut-(h*60*60))/60 );
	uint8_t s = ( ut-(h*60*60)-(m*60) );

	m_data.uptime.clear();
	m_data.uptime += (h<10)? "0" + QString::number(h):QString::number(h);m_data.uptime += ":";
	m_data.uptime += (m<10)? "0" + QString::number(m):QString::number(m);m_data.uptime += ":";
	m_data.uptime += (s<10)? "0" + QString::number(s):QString::number(s);
}

void HWMonitor::getCpu()
{
	static uint32_t diff_idle = 0;
	static uint32_t diff_total = 0;

	FILE* f = fopen("/proc/stat","r");
	QByteArray buff;
	char ch;
	uint8_t n;
	while( (n = fread(&ch,1,1,f) ) > 0 ){
		if(ch == '\n') break;
		buff.append(ch);
	}
	fclose(f);

	buff.replace(QByteArray("  "), QByteArray(" "));
	if( buff.indexOf("cpu ") == 0 ) buff.remove(0,4);
	auto tmp = buff.split(' ');

	if( tmp.size() != 10 ) return;

	uint32_t idle = tmp[3].toUInt();
	uint32_t total = 0;
	for(auto elem:tmp) total += elem.toUInt();

	diff_idle = idle - diff_idle;
	diff_total = total - diff_total;

	m_data.cpu = (float)( 1000*(diff_total-diff_idle)/diff_total+5 )/10;

	diff_idle = idle;
	diff_total = total;
}

void HWMonitor::getMem()
{
	QProcess cmd;
	cmd.start("free -b");
	cmd.waitForFinished(20);
	if( cmd.bytesAvailable() > 0 ){
		auto tmp = cmd.readAll().split('\n');
		if( tmp.size() != 4 ) return;
		while( tmp[1].contains( QByteArray("  ") ) ) tmp[1].replace("  ", QByteArray(" "));
		while( tmp[2].contains( QByteArray("  ") ) ) tmp[2].replace("  ", QByteArray(" "));
		auto tmpMem = tmp[1].split(' ');
		if( tmpMem.size() == 7 ){
			m_data.memTotal = mf::getSize( tmpMem[1].toLong() );
			m_data.memUsed = mf::getSize( tmpMem[2].toLong() );
			m_data.mem = tmpMem[2].toFloat() / ( tmpMem[1].toFloat() / 100.0 );
		}
		auto tmpSwap = tmp[2].split(' ');
		if( tmpSwap.size() == 7 ){
			m_data.swapTotal = mf::getSize( tmpSwap[1].toLong() );
			m_data.swapUsed = mf::getSize( tmpSwap[2].toLong() );
			m_data.swap = tmpSwap[2].toFloat() / ( tmpSwap[1].toFloat() / 100.0 );
		}
	}
}

void HWMonitor::getProcess()
{
	QDir dir = QDir("/proc");
	auto dirs = dir.entryList(QStringList() << "*",QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	uint16_t count = 0;
	for(auto elem:dirs){
		if( elem.toInt() > 0 ) count++;
	}
	m_data.procCount = QString::number(count);
}

void HWMonitor::getIfaces()
{
	NetIfList ifaces = NetIf::list();

	FILE* f = fopen("/proc/net/dev","r");
	QByteArray buff;
	char ch;
	uint8_t n;
	while( (n = fread(&ch,1,1,f) ) > 0 ) buff.append(ch);
	fclose(f);

	for(auto str:buff.split('\n')){
		if( !str.contains( QByteArray(": ") ) ) continue;
		if( str.contains( QByteArray(": ") ) ) str.replace(": ", QByteArray(" "));
		while( str.contains( QByteArray("  ") ) ) str.replace("  ", QByteArray(" "));
		if( str.indexOf(' ') == 0 ) str.remove(0,1);
		auto data = str.split(' ');
		if( data.size() < 10 ) continue;

		Iface iface;
		iface.name = data[0];
		iface.download = data[1].toLong();
		iface.upload = data[9].toLong();

		for(auto elem:ifaces){
			if( QByteArray(elem.name().c_str()) == iface.name ){
				iface.ip = QByteArray(elem.ip().toStr().c_str());
				iface.mac = QByteArray(elem.mac().toStr().c_str());
				break;
			}
		}


		bool find = false;
		for(auto &elem:m_data.ifaces){
			if(elem.name == iface.name){
				elem.downloadSpeed = iface.download - elem.download;
				elem.uploadSpeed = iface.upload - elem.upload;
				elem.download = iface.download;
				elem.upload = iface.upload;

				find = true;
				break;
			}
		}
		if(!find) m_data.ifaces.push_back(iface);

	}
}

void HWMonitor::getDevs()
{
	m_data.disks.clear();
	QProcess cmd;
	cmd.start("df -B 1");
	cmd.waitForFinished(20);
	if( cmd.bytesAvailable() > 0 ){
		auto buff = cmd.readAll();
		for(auto str:buff.split('\n')){
			while( str.contains( QByteArray("  ") ) ) str.replace("  ", QByteArray(" "));
			auto data = str.split(' ');
			if( data.size() != 6 ) continue;
			if( !data[0].contains('/') ) continue;

			Disk disk;
			disk.name = data[0];
			disk.size = data[1].toLong();
			disk.used = data[2].toLong();
			disk.avail = data[3].toLong();
			disk.mount = data[5];
			disk.usedPrz = (float)disk.used / ( (float)disk.size / 100.0 );

			m_data.disks.push_back(disk);
		}
	}
}
