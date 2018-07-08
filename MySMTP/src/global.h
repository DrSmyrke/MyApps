#ifndef GLOBAL_H
#define GLOBAL_H

#include <QByteArray>
#include <map>

struct SMTP_Config{
	uint16_t port = 2525;
};

struct SMTP_Pkt{
	QByteArray cmd;
	QByteArray data;
	bool valid = false;
};

namespace app {
	extern SMTP_Config smtpConf;
}
namespace smtp {
	SMTP_Pkt parsPkt(const QByteArray &data);
}

#endif // GLOBAL_H
