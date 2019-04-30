#include <QCoreApplication>
#include "server.h"
#include "global.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	//parsing args
	for(int i=0;i<argc;i++){
		if(QString(argv[i]).indexOf("-")==0){
			if(QString(argv[i]) == "--help" or QString(argv[1]) == "-h"){
				printf("Usage: SOCKSproxy.exe [OPTIONS] <LOCAL PORT>\n"
						//"SOCKS Proxy.\n"
						"  -l    Loging to file\n"
						"  -v    Verbose output\n"
						"  -f <FILE>     access file\n"
						"  -lp <LOGS PATH>     path for logs\n"
						"\n");
				return 0;
			}
			if(QString(argv[i]) == "-l") cfg::conf.logging=true;
			if(QString(argv[i]) == "-v") cfg::conf.verbose=true;
			if(QString(argv[i]) == "-f") cfg::conf.fileAccess=QString(argv[++i]);
			if(QString(argv[i]) == "-lp") cfg::conf.logsPath=QString(argv[++i]);
		}else{
			bool ok=false;
			QString(argv[i]).toInt(&ok,10);
			if(ok) cfg::conf.port=QString(argv[i]).toInt();
		}
	}

	Server* server=new Server();
	server->run();

	return a.exec();
}

