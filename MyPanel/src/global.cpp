#include "global.h"
#include <QSettings>

namespace app {
	Config conf;
	QSize screen;
	QString localeName;

	void loadSettings()
	{
		QSettings settings("MySoft","MyPanel");

		app::conf.sync.personalDir = settings.value("SYNC/personalDir",app::conf.sync.personalDir).toString();
		app::conf.sync.user = settings.value("SYNC/user",app::conf.sync.user).toString();
		app::conf.sync.server = settings.value("SYNC/server",app::conf.sync.server).toString();
		app::conf.sync.workDir = settings.value("SYNC/workDir",app::conf.sync.workDir).toString();
		app::conf.sync.port = settings.value("SYNC/port",app::conf.sync.port).toUInt();

		settings.beginGroup("SYNC_SAVE_DIRS");
		app::conf.sync.saveDirs.clear();
		for(auto elem:settings.childKeys()) app::conf.sync.saveDirs.push_back( settings.value(elem).toString() );
		settings.endGroup();

		settings.beginGroup("AUTOSTART");
		app::conf.autostartList.clear();
		for(auto elem:settings.childKeys()) app::conf.autostartList.push_back( settings.value(elem).toString() );
		settings.endGroup();

		settings.beginGroup("BOOKMARKS");
		app::conf.bookmarks.clear();
		for(auto elem:settings.childKeys()){
			auto tmp = settings.value(elem).toString().split("	");
			if( tmp.size() < 4 ) continue;

			Bookmark bm;
			bm.name = tmp[0];
			bm.type = tmp[1];
			bm.path = tmp[2];
			bm.mount = ( tmp[3].toUInt() == 1 )?true:false;
			if( tmp.size() >= 5 ) bm.mountDir = tmp[4];
			if( tmp.size() >= 6 ) bm.mountOnStart = ( tmp[5].toUInt() == 1 )?true:false;
			if( !bm.name.isEmpty() and !bm.type.isEmpty() and !bm.path.isEmpty() ) app::conf.bookmarks.push_back( bm );
		}
		settings.endGroup();
	}

	void saveSettings()
	{
		uint16_t i = 0;

		QSettings settings("MySoft","MyPanel");
		settings.clear();

		settings.setValue("SYNC/personalDir",app::conf.sync.personalDir);
		settings.setValue("SYNC/user",app::conf.sync.user);
		settings.setValue("SYNC/server",app::conf.sync.server);
		settings.setValue("SYNC/workDir",app::conf.sync.workDir);
		settings.setValue("SYNC/port",app::conf.sync.port);

		i = 0;
		for(auto elem:app::conf.sync.saveDirs){
			settings.setValue("SYNC_SAVE_DIRS/" + QString::number(i),elem);
			i++;
		}

		i = 0;
		for(auto elem:app::conf.autostartList){
			settings.setValue("AUTOSTART/" + QString::number(i),elem);
			i++;
		}

		i = 0;
		for(auto elem:app::conf.bookmarks){
			QString mount = ( elem.mount )?"1":"0";
			QString mountOnStart = ( elem.mountOnStart )?"1":"0";
			QString str;
			if( !elem.mountDir.isEmpty() and elem.mount ){
				str = elem.name + "	" + elem.type + "	" + elem.path + "	" + mount + "	" + elem.mountDir + "	" + mountOnStart;
			}else{
				str = elem.name + "	" + elem.type + "	" + elem.path + "	" + mount ;
			}
			settings.setValue("BOOKMARKS/" + QString::number(i),str);
			i++;
		}
	}

}

namespace mf {
	void removeDir(const QString &path)
	{
		if( !QDir( path ).exists() ) return;
		QDir dir = QDir( path );
		for(auto elem:dir.entryList(QStringList() << "*",QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)){
			if( QDir( path + "/" + elem ).exists() ){
				mf::removeDir( path + "/" + elem );
				continue;
			}
			QFile::remove( path + "/" + elem );
		}
		QDir().rmdir( path );
	}

	QString getSize(const long val)
	{
		QString str;
		if(val < 1024) str = QString::number(val) + " b";
		if(val >= 1024 && val < 1024000) str = QString::number(val/1024.0).left(5) + " Kb";
		if(val >= 1024000 && val < 1024000000) str =  QString::number(val/1048576.0).left(5) + " Mb";
		if(val >= 1048576000) str =  QString::number(val/1073741824.0).left(5) + " Gb";
		return str;
	}
}
