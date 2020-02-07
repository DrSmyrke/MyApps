#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDir>
#include <QRect>

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

struct SSHConfig{
	QString host;
	QString hostName;
	QString user;
	uint16_t port = 22;
	uint16_t defaultPort = 22;
	QString keyFile;
	std::pair<uint16_t, QString> localForward;
	std::vector<QString> kexAlgorithms;
};

struct SyncSettings{
	QString user;
	QString server;
	uint16_t port = 22;
	QString workDir;
	std::vector<QString> saveDirs;
	QString personalDir;
};

struct Bookmark{
	QString name;
	QString type;
	QString path;
	bool mount = false;
	QString mountDir;
	bool mountOnStart = false;
};

struct Valuta{
	float usd = 0;
	float eur = 0;
	QString dateUpdate;
};

//struct ApplicationData{
//	bool terminal = false;
//	QString name;
//	QString comment;
//	QString exec;
//	QString icon;
//};

struct Config{
	SyncSettings sync;
	QString sshConfDir = QDir::homePath()+"/.ssh";
	QString sshConfig = QDir::homePath()+"/.ssh/config";
	std::vector<Bookmark> bookmarks;
	Valuta valuta;
	bool verbose = false;
	uint8_t logLevel = 3;
	QString logFile;
};

namespace app {
	extern Config conf;
	extern QRect screen;
	extern QString localeName;

	void loadSettings();
	void saveSettings();
	bool parsArgs(int argc, char *argv[]);
	void setLog(const uint8_t logLevel, const QString &mess);
	bool startDetached(const QString &cmd, const QStringList &args = QStringList() , const QString &workPath = QDir::homePath());
}

namespace mf {
	void removeDir(const QString &path);
	QString getSize(const long val);
}

#endif // GLOBAL_H
