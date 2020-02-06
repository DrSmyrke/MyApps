#include "hwmonitor.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <sys/statfs.h>    /* or <sys/vfs.h> */
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>

HWMonitor::HWMonitor(QObject *parent) : QObject(parent)
{

}

void HWMonitor::slot_timer()
{
	static uint8_t counter = 0;

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
	QByteArray buff;
	QFile f("/proc/uptime");
	if( !f.exists() ) return;
	if( f.open( QIODevice::ReadOnly ) ){
		auto line = f.readLine().split(' ');
		f.close();
		buff.append( line[0] );
	}

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

	QByteArray buff;
	QFile f("/proc/stat");
	if( !f.exists() ) return;
	if( f.open( QIODevice::ReadOnly ) ){
		buff = f.readLine();
		f.close();
	}


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
	QByteArray buffmem;

	QFile fmem("/proc/meminfo");
	if( !fmem.exists() ) return;
	if( fmem.open( QIODevice::ReadOnly ) ){
		buffmem = fmem.readAll();
		fmem.close();
	}else{
		app::setLog( 2, QString("/proc/meminfo is NOT open [%1]").arg( fmem.errorString() ) );
		return;
	}

//	FILE* fmem = fopen("/proc/meminfo","r");
//	if( fmem ){
//		char ch;
//		uint8_t n;
//		while( (n = fread(&ch,1,1,fmem) ) > 0 ) buffmem.append(ch);
//		fclose(fmem);
//	}else{
//		return;
//	}


#if __WORDSIZE == 64
	uint64_t memTotal = 0;
	uint64_t memFree = 0;
	uint64_t swapTotal = 0;
	uint64_t swapFree = 0;
#else
	uint32_t memTotal = 0;
	uint32_t memFree = 0;
	uint32_t swapTotal = 0;
	uint32_t swapFree = 0;
#endif


	for(auto str:buffmem.split('\n')){
		while( str.contains( QByteArray("  ") ) ) str.replace("  ", QByteArray(" "));
		auto data = str.split(' ');
		if( data.size() != 3 ) continue;
		if( data[0].contains( QByteArray("MemTotal")  ) ) memTotal = data[1].toLong() * 1024;
		if( data[0].contains( QByteArray("MemAvailable")  ) ) memFree = data[1].toLong() * 1024;
		if( data[0].contains( QByteArray("SwapTotal")  ) ) swapTotal = data[1].toLong() * 1024;
		if( data[0].contains( QByteArray("SwapFree")  ) ) swapFree = data[1].toLong() * 1024;
	}
	m_data.memTotal = mf::getSize( memTotal );
	m_data.memUsed = mf::getSize( memTotal - memFree );
	m_data.mem = (float)(memTotal - memFree) / ( (float)memTotal / 100.0 );

	m_data.swapTotal = mf::getSize( swapTotal );
	m_data.swapUsed = mf::getSize( swapTotal - swapFree );
	m_data.swap = (float)(swapTotal - swapFree) / ( (float)swapTotal / 100.0 );

	// SWAPS
	//if( app::conf.swapMode == swap_mode_static ){
		m_data.swaps.clear();
		QByteArray buff;
		QFile f("/proc/meminfo");
		if( !f.exists() ) return;
		if( f.open( QIODevice::ReadOnly ) ){
			buff = f.readAll();
			f.close();
		}

		for(auto str:buff.split('\n')){
			str.replace("	", QByteArray(" "));
			while( str.contains( QByteArray("  ") ) ) str.replace("  ", QByteArray(" "));
			auto data = str.split(' ');
			if( data.size() != 5 ) continue;
			if( !data[0].contains( QByteArray("/") ) ) continue;
			m_data.swaps.push_back(data[0]);
		}
	//}
}

void HWMonitor::getProcess()
{
	QDir dir = QDir("/proc");
	auto dirs = dir.entryList( QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks );
	uint16_t count = 0;
	for(auto elem:dirs){
		if( elem.toInt() > 0 ) count++;
	}
	m_data.procCount = QString::number(count);
}

void HWMonitor::getIfaces()
{
	QByteArray buff;

//	FILE* f = fopen("/proc/net/dev","r");
//
//	if( f ){
//		char ch;
//		uint8_t n;
//		while( (n = fread(&ch,1,1,f) ) > 0 ) buff.append(ch);
//		fclose(f);
//	}

	QFile f("/proc/net/dev");
	if( !f.exists() ) return;
	if( f.open( QIODevice::ReadOnly ) ){
		buff = f.readAll();
		f.close();
	}else{
		app::setLog( 2, QString("/proc/net/dev is NOT open") );
		return;
	}

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

		auto sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP ) ;
		ifreq ifr ;
		strcpy( ifr.ifr_name, iface.name.toUtf8().data() ) ;
		// Получаем MAC-адрес интерфейса
		ioctl( sock, SIOCGIFHWADDR, &ifr ) ;
		char res[ 24 ] ;
		sprintf( res, "%02x:%02x:%02x:%02x:%02x:%02x",
			(unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[0],
			(unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[1],
			(unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[2],
			(unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[3],
			(unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[4],
			(unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[5] ) ;
		iface.mac = res;

		// Получаем IP-адрес интерфейса
		ioctl( sock, SIOCGIFADDR, &ifr ) ;
		//sprintf( res, "%s",.sa_data) ;
		iface.ip = inet_ntoa( ((struct sockaddr_in *) (&ifr.ifr_ifru.ifru_addr))->sin_addr );
		close( sock );

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
	QByteArray buff;
	m_data.disks.clear();
//	FILE* f = fopen("/proc/self/mountinfo","r");

//	if( f ){
//		char ch;
//		uint8_t n;
//		while( (n = fread(&ch,1,1,f) ) > 0 ) buff.append(ch);
//		fclose(f);
//	}

	QFile f("/proc/self/mountinfo");
	if( !f.exists() ) return;
	if( f.open( QIODevice::ReadOnly ) ){
		buff = f.readAll();
		f.close();
	}else{
		app::setLog( 2, QString("/proc/self/mountinfo is NOT open") );
		return;
	}

	for(auto str:buff.split('\n')){
		auto tmp = str.split(' ');
		if( tmp.size() < 10 ) continue;
		struct statfs fs;
		if( statfs( tmp[4].data(), &fs ) != 0 ) continue;

		Disk disk;
		disk.fstype = tmp[8];
		if( disk.fstype == "tmpfs" || disk.fstype == "debugfs" || disk.fstype == "fusectl"
				|| disk.fstype == "fusectl" || disk.fstype == "proc" || disk.fstype == "efivarfs"
				|| disk.fstype == "sysfs" || disk.fstype == "devtmpfs" || disk.fstype == "autofs"
				|| disk.fstype == "pstore" || disk.fstype == "binfmt_misc" || disk.fstype == "mqueue"
				|| disk.fstype == "fuse.gvfsd-fuse" || disk.fstype == "securityfs" || disk.fstype == "hugetlbfs"
				|| disk.fstype == "cgroup" || disk.fstype == "cgroup2" || disk.fstype == "devpts"
				|| disk.fstype == "configfs") continue;
		disk.name = tmp[9];
		disk.size = fs.f_bsize * fs.f_blocks;
		disk.avail = fs.f_bsize * fs.f_bavail;
		disk.mount = tmp[4];
		disk.used = disk.size - disk.avail;
		disk.usedPrz = (float)disk.used / ( (float)disk.size / 100.0 );
		m_data.disks.push_back(disk);
	}
}
