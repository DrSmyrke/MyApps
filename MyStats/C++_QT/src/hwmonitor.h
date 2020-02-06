#ifndef HWMONITOR_H
#define HWMONITOR_H

#include <QObject>
#include "global.h"
#include "myfunctions.h"

struct Iface{
	QString ip;
	QString mac;
	QString name;

#if __WORDSIZE == 64
	uint64_t upload = 0;
	uint64_t download = 0;
	uint64_t uploadSpeed = 0;
	uint64_t downloadSpeed = 0;
#else
	uint32_t upload = 0;
	uint32_t download = 0;
	uint32_t uploadSpeed = 0;
	uint32_t downloadSpeed = 0;
#endif
};

struct Disk{
	QString name;
	QString mount;
	QString fstype;
#if __WORDSIZE == 64
	uint64_t size = -1;
	uint64_t used = 0;
	uint64_t avail = 0;
#else
	uint32_t size = -1;
	uint32_t used = 0;
	uint32_t avail = 0;
#endif
	float usedPrz = 0;
};

struct UiData{
	QString time = "00:00:00";
	QString uptime = "00:00:00";
	QString date = "-- 00.00.0000";
	QString procCount = "0";
	float cpu = 0;
	float mem = 0;
	float swap = 0;
	QString memUsed = "0 b";
	QString memTotal = "0 b";
	QString swapUsed = "0 b";
	QString swapTotal = "0 b";
	std::vector<QString> swaps;
	std::vector<Iface> ifaces;
	std::vector<Disk> disks;
};

class HWMonitor : public QObject
{
	Q_OBJECT
public:
	explicit HWMonitor(QObject *parent = 0);
	UiData getData() { return m_data; }
public slots:
	void slot_timer();
private:
	UiData m_data;

	void getUptime();
	void getCpu();
	void getMem();
	void getProcess();
	void getIfaces();
	void getDevs();
};

#endif // HWMONITOR_H
