#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QTranslator>
#include <QLockFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QLockFile lockFile(QDir::homePath() + "/.mystats.lock");
	/* Пытаемся закрыть Lock File, если попытка безуспешна в течение 100 миллисекунд,
	 * значит уже существует Lock File созданный другим процессом.
	 * Следовательно, выбрасываем предупреждение и закрываем программу
	 * */

	auto localeName = QLocale::system().name();
	QTranslator translator(&a);
	if(translator.load(localeName,"://lang/")) a.installTranslator(&translator);

	if(!lockFile.tryLock(100)){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(QObject::tr("MyStats it`s work"));
		msgBox.exec();
		return 1;
	}

	MainWindow w;
	//w.show();

	return a.exec();
}
