#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDir>
#include <QSize>

enum{
	key_mode_ctrl = 0x01,
	key_mode_lalt,
	key_mode_rwin,
	key_type_R = 0x0052,
	key_type_E = 0x0045,
	key_type_Tilda = 0x007e,
	swap_mode_static = 3,
	swap_mode_dynamic = 4,
};

enum{
	ui_skrepka,
};

struct SerialPortSettings{
	QString port;
	uint32_t speed = 9600;
	int8_t dataBits = 8;
	int8_t parity = 0;
	int8_t stopBits = 1;
	int8_t folowControl = 0;
};

struct SyncSettings{
	QString user;
	QString server;
	uint16_t port = 22;
	std::vector<QString> saveDirs;
	QString saveToDir;
};

struct Config{
	SerialPortSettings serialPort;
	QString serialMonitor;
	std::vector<QString> autostartList;
	SyncSettings sync;
	uint8_t swapMode = swap_mode_dynamic;
};

namespace app {
	extern Config conf;
	extern QSize screen;

	void loadSettings();
	void saveSettings();
}

namespace mf {
	void removeDir(const QString &path);
	QString getSize(const long val);
}

#endif // GLOBAL_H
