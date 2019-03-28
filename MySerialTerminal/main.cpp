#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "global.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	auto localeName = QLocale::system().name();
	QTranslator translator(&a);
	if(translator.load(localeName,"://lang/")) a.installTranslator(&translator);

	//app::loadSettings();
	if( !app::parsArgs(argc, argv) ) return 0;

	MainWindow w;
	w.show();
	w.run();

	return a.exec();
}
