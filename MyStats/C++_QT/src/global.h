#include <QPoint>
#include <QString>

#ifndef GLOBAL_H
#define GLOBAL_H

struct Config{
	bool showData = false;
	bool verbose			= true;
	uint8_t logLevel		= 3;
	QString logFile			= "/tmp/mystats.log";
};

namespace app {
	extern Config conf;
}

#endif // GLOBAL_H
