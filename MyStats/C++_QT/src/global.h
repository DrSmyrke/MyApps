#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QDir>
#include <QHostAddress>

struct Config{
	bool showData = false;
	bool verbose			= true;
	uint8_t logLevel		= 3;
	QString logFile			= "/tmp/mystats.log";
};

namespace app {
	extern Config conf;

	void setLog(const uint8_t logLevel, const QString &mess);
}

#endif // GLOBAL_H
