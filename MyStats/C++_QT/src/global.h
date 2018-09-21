#include <QPoint>
#include <QString>

#ifndef GLOBAL_H
#define GLOBAL_H

enum{
	window_state_hide,
	window_state_show,
};
struct Config{
	bool fixed = false;
	uint16_t borderSize = 10;
	uint16_t showSize = 5;
	QPoint winPos;
	uint8_t saveSettingsCounter = 0;
	bool saveSettings = false;
	int16_t hideWindowCounter = -1;
	uint8_t hideWindowCounterMax = 2;
	uint16_t windowFixedWidth = 270;
	uint16_t windowRealHeight = 240;
	uint8_t windowState = window_state_show;
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
