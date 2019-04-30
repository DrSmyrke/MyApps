#include "myfunctions.h"
#include "global.h"

#include <QDataStream>
#include <QIODevice>
#include <QCryptographicHash>
#include <QProcess>
#include <QDateTime>

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

	QByteArray toBigEndianInt(const int val)
	{
		QByteArray bytes(4, 0x00);
		QDataStream stream(&bytes, QIODevice::WriteOnly);
		stream.setByteOrder(QDataStream::BigEndian);
		stream << val;
		bytes.resize(4);
		return bytes;
	}

	QByteArray toBigEndianShort(const short val)
	{
		QByteArray bytes(2, 0x00);
		QDataStream stream(&bytes, QIODevice::WriteOnly);
		stream.setByteOrder(QDataStream::BigEndian);
		stream << val;
		bytes.resize(2);
		return bytes;
	}

	unsigned int getRand(const int max)
	{
		return qrand() % max;
	}

	QByteArray md5(const QByteArray &string)
	{
		return QCryptographicHash::hash(string,QCryptographicHash::Md5);
	}

	bool startDetached(const QString &cmd, const QStringList &args)
	{
		QString str = cmd + " " + args.join(" ");
		auto res = QProcess::startDetached( cmd, args );
		mf::setLog(1,QString("[EXEC %1]: %2").arg( (res)?"true":"false" ).arg(str));
		return res;
	}

	void setLog(const uint8_t logLevel, const QString &mess)
	{
		if(app::conf.logLevel < logLevel or app::conf.logLevel == 0) return;

		QDateTime dt = QDateTime::currentDateTime();
		QString str = dt.toString("yyyy.MM.dd [hh:mm:ss] ") + mess + "\n";

		if( app::conf.verbose ){
			printf( "%s", str.toUtf8().data() );
			fflush( stdout );
		}

		if( app::conf.logFile.isEmpty() ) return;
		FILE* f;
		f = fopen(app::conf.logFile.toUtf8().data(),"a+");
		fwrite(str.toUtf8().data(),str.length(),1,f);
		fclose(f);
	}

}