#include "dataupdate.h"
#include <QDateTime>
#include <QSettings>
#include <QXmlStreamReader>

DataUpdate::DataUpdate(QObject *parent) : QObject(parent)
{
	m_pWget = new Wget(this);
	m_pTimer = new QTimer(this);
		m_pTimer->setInterval(3600000);
		m_pTimer->start();

	connect(m_pTimer,&QTimer::timeout,this,&DataUpdate::updateValuta);
	connect(m_pWget,&Wget::signal_complete,this,[this](){
		switch (m_wgetOperation) {
			case wget_operation_getKotirovki:
				parsingValuta( m_pWget->getBody() );
				//if( m_updateAllF ) updateWeather();
			break;
		}
	});
}

void DataUpdate::slot_stop()
{
	m_pWget->deleteLater();
	m_pTimer->deleteLater();
	emit signal_finished();
}

void DataUpdate::updateValuta()
{
	QDateTime dt = QDateTime::currentDateTime();
	auto date = dt.toString("dd/MM/yyyy");
	app::conf.valuta.dateUpdate = date;
	if( m_pWget->isRunning() ) return;
	m_wgetOperation = wget_operation_getKotirovki;
	m_pWget->get( "http://www.cbr.ru/scripts/XML_daily.asp?date_req=" + date );
}

void DataUpdate::parsingValuta(const QString &data)
{
	QXmlStreamReader xml(data);
	while (!xml.atEnd() && !xml.hasError()){
		QXmlStreamReader::TokenType token = xml.readNext();
		if (token == QXmlStreamReader::StartDocument) continue;
		if (token == QXmlStreamReader::StartElement){
			if (xml.name() == "Valute"){
				QString val;
				int nominal = 0;
				float value;
				while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Valute")){
					if (xml.tokenType() == QXmlStreamReader::StartElement){
						if (xml.name() == "CharCode"){
							if (xml.tokenType() == QXmlStreamReader::StartElement){
								xml.readNext();
								val = xml.text().toString();
							}
						}
						if (xml.name() == "Nominal"){
							if (xml.tokenType() == QXmlStreamReader::StartElement){
								xml.readNext();
								nominal = xml.text().toUInt();
							}
						}
						if (xml.name() == "Value"){
							if (xml.tokenType() == QXmlStreamReader::StartElement){
								xml.readNext();
								value = xml.text().toString().replace(",",".").toFloat();
							}
						}
					}
					xml.readNext();
				}
				if(val == "USD") app::conf.valuta.usd = (float)nominal*value;
				if(val == "EUR") app::conf.valuta.eur = (float)nominal*value;
			}
		}
	}
}

//void DataUpdate::applicationsUpd()
//{
//	app::menuAplicationsDB.media.clear();
//	app::menuAplicationsDB.graphics.clear();
//	app::menuAplicationsDB.network.clear();
//	app::menuAplicationsDB.office.clear();
//	app::menuAplicationsDB.dev.clear();
//	app::menuAplicationsDB.settings.clear();
//	app::menuAplicationsDB.system.clear();
//	app::menuAplicationsDB.other.clear();

//	QString path = "/usr/share/applications";
//	QDir dir = QDir( path );
//	for(auto elem:dir.entryList(QStringList() << "*.desktop",QDir::NoSymLinks | QDir::Files | QDir::NoDotAndDotDot)){
//		QSettings ini(  path + "/" + elem, QSettings::IniFormat );
//		ApplicationData appData;
//			appData.icon = ini.value("Desktop Entry/Icon","").toString();
//			appData.terminal = ini.value("Desktop Entry/Terminal","").toBool();
//			appData.exec = ini.value("Desktop Entry/Exec","").toString();
//			appData.name = ini.value("Desktop Entry/Name","").toString().toUtf8();
//			if( appData.name.isEmpty() or appData.exec.isEmpty() ) continue;
//		bool find = false;
//		for(auto cat:ini.value("Desktop Entry/Categories",tr("Other")).toString().split(";")){
//			if( cat.toLower() == "system" ){
//				app::menuAplicationsDB.system.push_back( appData );
//				find = true;
//				break;
//			}
//			if( cat.toLower() == "audiovideo" ){
//				app::menuAplicationsDB.media.push_back( appData );
//				find = true;
//				break;
//			}
//			if( cat.toLower() == "settings" ){
//				app::menuAplicationsDB.settings.push_back( appData );
//				find = true;
//				break;
//			}
//			if( cat.toLower() == "graphics" ){
//				app::menuAplicationsDB.graphics.push_back( appData );
//				find = true;
//				break;
//			}
//			if( cat.toLower() == "network" ){
//				app::menuAplicationsDB.network.push_back( appData );
//				find = true;
//				break;
//			}
//			if( cat.toLower() == "system" ){
//				app::menuAplicationsDB.system.push_back( appData );
//				find = true;
//				break;
//			}
//			if( cat.toLower() == "development" || cat.toLower() == "qt" ){
//				app::menuAplicationsDB.dev.push_back( appData );
//				find = true;
//				break;
//			}
//			if( cat.toLower() == "office" || cat.toLower() == "qt" ){
//				app::menuAplicationsDB.office.push_back( appData );
//				find = true;
//				break;
//			}
//		}
//		if( !find ) app::menuAplicationsDB.other.push_back( appData );
//	}
//	emit signal_applicationUpdate();
//}
