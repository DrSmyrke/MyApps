#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLockFile>
#include <QMessageBox>
#include "global.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QLockFile lockFile(QDir::temp().absoluteFilePath("mypanel.lock"));
	/* Пытаемся закрыть Lock File, если попытка безуспешна в течение 100 миллисекунд,
	 * значит уже существует Lock File созданный другим процессом.
	 * Следовательно, выбрасываем предупреждение и закрываем программу
	 * */
	if(!lockFile.tryLock(100)){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText("Приложение уже запущено.\n"
					   "Разрешено запускать только один экземпляр приложения.");
		msgBox.exec();
		return 1;
	}

	app::localeName = QLocale::system().name();
	QTranslator translator(&a);
	if(translator.load(app::localeName,"://lang/")) a.installTranslator(&translator);

	app::loadSettings();
	if( !app::parsArgs(argc, argv) ) return 0;

	MainWindow w;
	w.show();
    w.run();

	return a.exec();
}
