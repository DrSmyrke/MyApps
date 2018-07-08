#include <QCoreApplication>
#include "global.h"
#include "smtp_server.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	SMTP_Server smtpServer;

	smtpServer.start();

	return a.exec();
}

