#include "global.h"
#include <QSettings>

namespace app {
	Config conf;

	void loadSettings()
	{
		QSettings settings("MySoft","MyStats");
		app::conf.fixed = settings.value("UI/fixed", app::conf.fixed).toBool();
		app::conf.borderSize = settings.value("UI/borderSize", app::conf.borderSize).toUInt();
		app::conf.showSize = settings.value("UI/showSize", app::conf.showSize).toUInt();
		app::conf.winPos.setX( settings.value("UI/winPosX", app::conf.winPos.x()).toInt() );
		app::conf.winPos.setY( settings.value("UI/winPosY", app::conf.winPos.y()).toInt() );
		app::conf.fixed = settings.value("UI/fixed", app::conf.fixed).toBool();

		if( settings.childGroups().size() == 0 ) app::saveSettings();
	}

	void saveSettings()
	{
		QSettings settings("MySoft","MyStats");
		settings.clear();
		settings.setValue("UI/fixed", app::conf.fixed);
		settings.setValue("UI/borderSize", app::conf.borderSize);
		settings.setValue("UI/showSize", app::conf.showSize);
		settings.setValue("UI/winPosX", app::conf.winPos.x());
		settings.setValue("UI/winPosY", app::conf.winPos.y());
		settings.setValue("UI/fixed", app::conf.fixed);
	}
}

namespace mf {
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
