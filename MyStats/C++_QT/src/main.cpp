#include "mainwindow.h"
#include <QApplication>
#include "global.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	app::loadSettings();

	MainWindow w;
	w.show();

	return a.exec();
}
