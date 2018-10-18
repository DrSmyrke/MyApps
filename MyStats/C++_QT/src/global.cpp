#include "global.h"
#include <QSettings>

namespace app {
	Config conf;
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
