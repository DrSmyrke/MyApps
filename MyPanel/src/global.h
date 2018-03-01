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
	sync_mode_work = 1,
	sync_mode_home = 2,
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
	uint8_t mode = 0;
	QString remoteDir;
	QString user;
	QString server;
	uint16_t port = 22;
	std::vector<QString> dirs;
	std::vector<QString> saveDirs;
};

struct Config{
	SerialPortSettings serialPort;
	QString serialMonitor;
	std::vector<QString> autostartList;
	SyncSettings sync;
};

namespace app {
	extern Config conf;
	extern QSize screen;

	void loadSettings();
	void saveSettings();
}

namespace mf {
	void removeDir(const QString &path);
	QString modeToStr(const uint8_t mode);
	QString getSize(const long val);
}

#endif // GLOBAL_H
