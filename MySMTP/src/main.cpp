#include <QCoreApplication>
#include "global.h"
#include "smtp_server.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	app::loadSettings();
	if( !app::parsArgs(argc, argv) ) return 0;

	SMTP_Server server;
	if( !server.start() ) return 1;

	return a.exec();
}

