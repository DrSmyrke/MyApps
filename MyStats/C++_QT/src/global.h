#include <QPoint>
#include <QString>

#ifndef GLOBAL_H
#define GLOBAL_H

struct Config{
	bool fixed = false;
	uint16_t borderSize = 10;
	uint16_t showSize = 5;
	QPoint winPos;
	uint8_t saveSettingsCounter = 0;
	bool saveSettings = false;
};

namespace app {
	extern Config conf;
	void loadSettings();
	void saveSettings();
}

namespace mf {
	QString getSize(const long val);
}

#endif // GLOBAL_H
