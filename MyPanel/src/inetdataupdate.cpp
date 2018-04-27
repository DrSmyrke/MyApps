#include "inetdataupdate.h"
#include <QDateTime>
#include <QXmlStreamReader>
//TODO: remove qdebug
#include <QDebug>

InetDataUpdate::InetDataUpdate(QObject *parent) : QObject(parent)
{
	m_pWget = new Wget(this);
	m_pTimer = new QTimer(this);
		m_pTimer->setInterval(3600000);
		m_pTimer->start();

	connect(m_pTimer,&QTimer::timeout,this,&InetDataUpdate::updateAll);
	connect(m_pWget,&Wget::signal_complete,this,[this](){
		switch (m_wgetOperation) {
			case wget_operation_getKotirovki:
				parsingValuta( m_pWget->getBody() );
				//if( m_updateAllF ) updateWeather();
			break;
		}
	});
}

void InetDataUpdate::updateAll()
{
	updateValuta();
	m_updateAllF = true;
}

void InetDataUpdate::updateValuta()
{
	QDateTime dt = QDateTime::currentDateTime();
	auto date = dt.toString("dd/MM/yyyy");
	app::conf.valuta.dateUpdate = date;
	if( m_pWget->isRunning() ) return;
	m_wgetOperation = wget_operation_getKotirovki;
	m_pWget->get( "http://www.cbr.ru/scripts/XML_daily.asp?date_req=" + date );
}

void InetDataUpdate::parsingValuta(const QString &data)
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
