#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QTranslator>
#include "../../../HomeNET/client.h"
#include "../../../HomeNET/searcher.h"
#include "../../../HomeNET/myproto.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	auto localeName = QLocale::system().name();
	QTranslator translator(&a);
	if(translator.load(localeName,"://lang/")) a.installTranslator(&translator);

	myproto::appData.type = app_type_mystats;
	//Searcher searcher(&a);
	//Client client( socket_type_client, &a );
	//client.open( QHostAddress::LocalHost, myproto::conf.port );

	app::conf.showData = true;

	MainWindow w;
	w.show();

	//searcher.start();

	return a.exec();
}
