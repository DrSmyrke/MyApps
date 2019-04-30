#include "global.h"

#include <QDateTime>
#include <QDir>
#include <QSettings>

namespace app {
	SMTP_Config conf;

	void loadSettings()
	{
		if( app::conf.confFile.isEmpty() ) return;

		QSettings sett(app::conf.confFile, QSettings::IniFormat);

		app::conf.verbose = sett.value( "Main/verbose", app::conf.verbose ).toBool();
		app::conf.port = sett.value( "Main/port", app::conf.port ).toUInt();
		app::conf.logLevel = sett.value( "Main/logLevel", app::conf.logLevel ).toUInt();
		app::conf.logFile = sett.value( "Main/logFile", app::conf.logFile ).toString();
		app::conf.mailsFolder = sett.value("Main/mailsFolder",app::conf.mailsFolder).toString();

		sett.beginGroup("BLACKLIST");
		app::conf.blackMails.clear();
		for(auto elem:sett.childKeys()) app::conf.blackMails.push_back( sett.value(elem).toString() );
		sett.endGroup();

		app::setLog(1,QString("app:loadSettings from [%1]").arg(app::conf.confFile));

		if( sett.allKeys().size() == 0 ) app::saveSettings();
	}

	void saveSettings()
	{
		if( app::conf.confFile.isEmpty() ) return;

		app::setLog(1,QString("app:saveSettings to [%1]").arg(app::conf.confFile));

		//check && create confDir
		auto tmp = app::conf.confFile.split("/");
		tmp.pop_back();
		QString dir = tmp.join("/");
		if( !QDir(dir).exists() ) QDir().mkpath( dir );
		if( !QDir(dir).exists() ) app::setLog(1,QString("app:saveSettings cannot create dir [%1]").arg(dir));
		//
		QSettings sett(app::conf.confFile, QSettings::IniFormat);
		if( !sett.isWritable() ){
			app::setLog(1,QString("app:saveSettings cannot writable [%1]").arg(app::conf.confFile));
			return;
		}

		sett.setValue("Main/verbose",app::conf.verbose);
		sett.setValue("Main/port",app::conf.port);
		sett.setValue("Main/threads",app::conf.mailsFolder);
		sett.setValue("Main/logLevel",app::conf.logLevel);
		sett.setValue("Main/logFile",app::conf.logFile);

		uint16_t i = 0;
		for(auto elem:app::conf.blackMails) sett.setValue("BLACKLIST/" + QString::number(i++),elem);
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

	bool isBlack(const QString &email)
	{
		bool ret = false;
		for(auto elem:app::conf.blackMails){
			if( email == elem ){
				ret = false;
				break;
			}
		}
		return ret;
	}

	bool parsArgs(int argc, char *argv[])
	{
		bool ret = true;
		for(int i=0;i<argc;i++){
			if(QString(argv[i]).indexOf("-")==0){
				if(QString(argv[i]) == "--help" or QString(argv[1]) == "-h"){
					printf("Usage: %s [OPTIONS] <LOCAL PORT>\n"
							"  -c <FILE>    config file\n"
							"  -v    Verbose output\n"
							"\n", argv[0]);
					ret = false;
				}
				if(QString(argv[i]) == "-c"){
					app::conf.confFile = QString(argv[++i]);
					app::loadSettings();
				}
				if(QString(argv[i]) == "-v") app::conf.verbose = true;
			}else{
				bool ok = false;
				QString(argv[i]).toInt(&ok,10);
				if(ok) app::conf.port = QString(argv[i]).toInt();
			}
		}
		return ret;
	}

	QByteArray base64_decode(const QString &string)
	{
		QByteArray ba;
		ba.append(string);
		return QByteArray::fromBase64(ba);
	}

	QByteArray quotedPrintable_decode(const QString &input)
	{
		QByteArray output;
		for (int i = 0; i < input.length(); ++i){
			if(input.at(i)=='\r') continue;

			if(input.at(i)=='='){
				//FIXME: костыль
				QString str=input.at(++i);
				str+=input.at(++i);
				if(str == "\n\r" or str == "\r\n") continue;
				output.append(str.toInt(nullptr,16));
			}else{
				output.append(input.at(i));
			}
		}
		return output;
	}

	void saveNewMess(const Mail &mail)
	{
		for(auto to:mail.to){
			QString path = app::conf.mailsFolder + "/" + to;
			if( !QDir(path).exists() ) QDir().mkpath( path );
			if( !QDir(path).exists() ) app::setLog(1,QString("app:saveSettings cannot create dir [%1]").arg(path));

			auto currentTime=QTime::currentTime();
			auto currentDate = QDate::currentDate();
			auto time=currentTime.toString("hh:mm:ss");
			QString date=currentDate.toString("yyyy_MM_dd")+" ["+time+"]";
			QFile file(path + "/" + date + ".eml");
			if (file.open(QIODevice::WriteOnly)){
				file.write(QString(mail.from + "\n").toUtf8());
				file.write(QString(mail.theme + "\n").toUtf8());
				file.write(QString("==============\n").toUtf8());
				file.write(QString(mail.message + "\n").toUtf8());
				file.write(QString("==============\n").toUtf8());
				//file.write(QString(mail.messageNew + "\n").toUtf8());
				//file.write(QString("==============\n").toUtf8());
				file.close();
			}
		}
	}

}
namespace smtp {

	SMTP_Pkt parsPkt(const QByteArray &data)
	{
		SMTP_Pkt pkt;

		for(uint32_t i = 0; i < data.size(); i++){
			if( pkt.cmd.size() == 0 && data.at(i) == ' ' ){
				pkt.cmd.append( pkt.data );
				pkt.data.clear();
				continue;
			}
			if( pkt.cmd == "MAIL" && data.at(i) == ':' && pkt.data == "FROM"){
				pkt.cmd.append( pkt.data );
				pkt.data.clear();
				continue;
			}
			if( pkt.cmd == "RCPT" && data.at(i) == ':' && pkt.data == "TO"){
				pkt.cmd.append( pkt.data );
				pkt.data.clear();
				continue;
			}
			pkt.data.append( data[i] );
		}

		if( pkt.cmd.size() > 0 and pkt.data.right( 2 ) == "\r\n" ) pkt.valid = true;
		if( pkt.data.right( 2 ) == "\r\n" ) pkt.data.remove( pkt.data.size() - 2, 2 );

		return pkt;
	}

	QString parsEmail(const QString &email)
	{
		QString str = email;
		str.replace("<","");
		str.replace(">","");
		str.replace("\r","");
		str.replace("\n","");
		return str;
	}

	QString getTheme(const QString &mess)
	{
		QString theme;
		for(auto str:mess.split("\n")){
			if(str.indexOf("Subject: ",Qt::CaseInsensitive)==0){
				auto tmp=str.split(": ");
				str.remove(0,tmp[0].length()+2);
				str.replace("\r","");
				for(auto elem:str.split(" ")){
					if(elem.contains("=?",Qt::CaseInsensitive) and elem.contains("?=",Qt::CaseInsensitive)){
						tmp=elem.split("?");
						if(tmp.size()>2 and tmp[2].contains("b",Qt::CaseInsensitive)) elem = app::base64_decode(tmp[3]);
						if(tmp.size()>2 and tmp[2].contains("q",Qt::CaseInsensitive)) elem = app::quotedPrintable_decode(tmp[3]);
						if(tmp[1].contains("utf-8",Qt::CaseInsensitive)) elem=elem.toUtf8();
					}
					theme += elem+" ";
				}
				break;
			}
		}
		return theme;
	}

	QString parsBody(const QString &data)
	{
		QString body;
		Head head;

		QByteArray buff;
		QString param;
		unsigned int headEndPos = 0;
		for(unsigned int i = 0; i < data.length(); i++){
			if(data[i] == '\r') continue;

			if(buff.right(2) == "\n\n"){
				headEndPos = i;
				break;
			}
			if(param.isEmpty() and buff.right(1) == ":" and data[i] == ' '){
				param = buff;
				buff.clear();
				continue;
			}
			if(data[i] == '\n'){
				if(!param.isEmpty()){
					if(param.contains("content-type",Qt::CaseInsensitive)) head.contentType = buff;
					if(param.contains("content-transfer-encoding",Qt::CaseInsensitive)) head.contentEncoding = buff;

					buff.clear();
					param.clear();
				}
			}

			buff.append(data.at(i));
		}
		//qDebug()<<"MAIL	head.contentType ="<<head.contentType;
		//qDebug()<<"MAIL	head.contentEncoding ="<<head.contentEncoding;
		for(unsigned int i = headEndPos; i < data.length(); i++){
			body += data.at(i);
		}
		if(head.contentEncoding.contains("quoted-printable",Qt::CaseInsensitive)){
			body = app::quotedPrintable_decode(body);
		}
		return body;
	}

}
