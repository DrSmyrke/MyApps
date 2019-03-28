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
	ui_skrepka,
	proc_status_normal,
	proc_status_crash,
	proc_status_exit,
};

struct SerialSettings{
	QString port;
	uint32_t speed = 9600;
	int8_t bits = 8;
	int8_t flow = 0;
	int8_t parity = 0;
	int8_t stopBits = 1;
};

struct Config{
	SerialSettings serial;
	bool verbose = false;
	uint8_t logLevel = 3;
	QString logFile;
};

namespace app {
	extern Config conf;

	void loadSettings();
	void saveSettings();
	bool parsArgs(int argc, char *argv[]);
	void setLog(const uint8_t logLevel, const QString &mess);
	void startDetached( const QString &cmd, const QStringList &args = QStringList() );
}

namespace mf {
	void removeDir(const QString &path);
	QString getSize(const long val);
}

#endif // GLOBAL_H
