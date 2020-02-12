#ifndef GLOBAL_H
#define GLOBAL_H

#include <QByteArray>
#include <QString>
#include <map>
#include <vector>

struct SMTP_Config{
	uint16_t port = 2525;
	QString confFile = "/etc/DrSmyrke/smtp.cfg";
	std::vector<QString> blackMails;
	QString mailsFolder = "/my/mails";
	uint8_t logLevel = 3;
	QString logFile;
	bool verbose = false;
};

struct Head{
	QString contentType;
	QString contentEncoding;
};

struct Mail{
	QString message;
	QString messageNew;
	QString from;
	QString theme;
	bool read = false;
	std::vector<QString> to;
};

struct SMTP_Pkt{
	QByteArray cmd;
	QByteArray data;
	bool valid = false;
};

namespace app {
	extern SMTP_Config conf;

	void loadSettings();
	void saveSettings();
	void setLog(const uint8_t logLevel, const QString &mess);
	bool isBlack(const QString &email);
	bool parsArgs(int argc, char *argv[]);
	QByteArray base64_decode(const QString &string);
	QByteArray quotedPrintable_decode(const QString &input);
	void saveNewMess(const Mail &mail);
}
namespace smtp {
	SMTP_Pkt parsPkt(const QByteArray &data);
	QString parsEmail(const QString &email);
	QString getTheme(const QString &mess);
	QString parsBody(const QString &data);
}

#endif // GLOBAL_H
