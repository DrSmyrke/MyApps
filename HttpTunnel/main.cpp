#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	if((argc != 4) or QString(argv[1]) == "--help" or QString(argv[1]) == "-h"){
		printf("Usage: HttpTunnel.exe <LOCAL PORT> <REMOTE HOST> <REMOTE PORT>\n"
				//"Http Tunnel.\n"
//				"  -map    set the map\n"
//				"  -servername     set the name to server\n"
				"\n");
		return 1;
	}

	Server server(QString(argv[1]).toInt(),QString(argv[3]).toInt(),QString(argv[2]));
	server.start();

	return a.exec();
}

