#include <QPoint>
#include <QString>

#ifndef GLOBAL_H
#define GLOBAL_H

struct Config{
	bool showData = false;
};

namespace app {
	extern Config conf;
}

namespace mf {
	QString getSize(const long val);
}

#endif // GLOBAL_H
