#include "execwindow.h"
#include <QLayout>
#include <QCompleter>
#include <QDateTime>
#include <unistd.h>

//TODO: remove qdebug
#include <QDebug>

ExecWindow::ExecWindow(QWidget *parent) : QMainWindow(parent)
{
	m_pStringListModel = new QStringListModel(this);
	m_pCompleter = new QCompleter(this);
		m_pCompleter->setCompletionMode(QCompleter::PopupCompletion);
		m_pCompleter->setModel( m_pStringListModel );
		connect(m_pCompleter,SIGNAL(highlighted(QString)),this,SLOT(slot_highlighted(QString)));
	m_pExecLine = new QLineEdit(this);
		m_pExecLine->setCompleter(m_pCompleter);

//	QWidget* centrWidget = new QWidget(this);
//		QHBoxLayout* hBox = new QHBoxLayout();
//			hBox->setSpacing(0);
//			hBox->setMargin(0);
//		hBox->addWidget(m_pExecLine);
//	centrWidget->setLayout(hBox);

	setCentralWidget(m_pExecLine);
	setWindowFlags(Qt::Popup);
	//setAttribute(Qt::WA_TranslucentBackground);
	setFixedSize(350,30);

	connect(m_pExecLine,&QLineEdit::textChanged,this,&ExecWindow::slot_textChanged);
	connect(m_pExecLine,&QLineEdit::returnPressed,this,&ExecWindow::slot_returnPressed);
}

void ExecWindow::open()
{
	if( this->isHidden() ){
		this->show();
	}else{
		this->hide();
		return;
	}

	this->setFocus();
	m_pExecLine->setFocus();
}

void ExecWindow::slot_textChanged(const QString &text)
{
	if( m_flag ){
		m_flag = false;
		return;
	}
	QStringList list;

	if( text.left(1) == "/" ){
		QStringList tmp = text.split("/");
		QString last = tmp[tmp.size()-1];
		tmp.pop_back();
		QString path = tmp.join('/');
		if( path.isEmpty() ) path = "/";
		QDir dir( path );
		for(auto elem:dir.entryList( QStringList() << last + "*",QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)){
			if( path == "/" ) path.clear();
			list.push_back( path + "/" + elem );
		}

		m_pStringListModel->setStringList( list );
		return;
	}

	if( text.left(2) == "c " or text.left(2) == "с " or text.left(3) == "lj " or text.left(3) == "до " ){
		bool inversionF = ( text.left(3) == "lj " or text.left(3) == "до " )?true:false;
		auto tmp = text.mid(2,text.length()-2).split(".");
		if( tmp.size() > 0 ){

			QDate cdt = QDate::currentDate();

			uint16_t	year = ( tmp.size() > 2 and tmp[2].toUInt() ) ? tmp[2].toUInt(): cdt.year();
			uint8_t		mon = ( tmp.size() > 1 and tmp[1].toUInt() ) ? tmp[1].toUInt(): cdt.month();
			uint8_t		day = ( tmp[0].toUInt() ) ? tmp[0].toUInt(): cdt.day();

			QDate startDate( year, mon, day );
			QDate endDate( QDate::currentDate() );
			QString str = text.left(2);
			str += tmp[0];
			str += ".";
			if( tmp.size() > 1 ){
				str += tmp[1];
			}else{
				str += ( mon < 10 ) ? "0" + QString::number(mon) : QString::number(mon);
			}
			str += ".";
			str += QString::number(year);
			str += " ";

			int eDays = ( inversionF )? 0 - startDate.daysTo(endDate) : startDate.daysTo(endDate);
			str += ( inversionF ) ? tr("left ") : tr("passed ");
			list.push_back( str + tr("days: ") + QString::number(eDays) );

			int w = eDays / 7;
			if( w ){
				list.push_back( str + tr("weeks: ") + QString::number(w) );
//				int rDay = eDays - w*7;
//				QString tmpStr = str + tr("weeks: ") + QString::number(w);
//				if( rDay ) tmpStr += " " + tr("days: ") + QString::number(rDay);
//				list.push_back( tmpStr );
			}
			int m = eDays / 30;
			if( m ) list.push_back( str + tr("months: ") + QString::number(m) );
			int y = m / 12;
			if( y ) list.push_back( str + tr("years: ") + QString::number(y) );

			m_pStringListModel->setStringList( list );
		}
		return;
	}

	// MONEY

	auto tmp = text.split(" ");
	if( tmp.size() >= 2 ){
		if( tmp[1].toLower() == "usd" or tmp[1].toLower() == "$" ){
			if( !app::conf.valuta.usd or !app::conf.valuta.eur ) return;
			list.push_back( text + " = " + QString::number(tmp[0].toFloat() * app::conf.valuta.usd) + " ₽" );
			list.push_back( text + " = " + QString::number(tmp[0].toFloat() * app::conf.valuta.usd / app::conf.valuta.eur) + " €" );
			m_pStringListModel->setStringList( list );
			return;
		}
		if( tmp[1].toLower() == "eur" or tmp[1].toLower() == "€" ){
			if( !app::conf.valuta.usd or !app::conf.valuta.eur ) return;
			list.push_back( text + " = " + QString::number(tmp[0].toFloat() * app::conf.valuta.eur) + " ₽" );
			list.push_back( text + " = " + QString::number(tmp[0].toFloat() * app::conf.valuta.eur / app::conf.valuta.usd) + " $" );
			m_pStringListModel->setStringList( list );
			return;
		}
		if( tmp[1].toLower() == "rub" or tmp[1].toLower() == "₽" ){
			if( !app::conf.valuta.usd or !app::conf.valuta.eur ) return;
			list.push_back( text + " = " + QString::number(tmp[0].toFloat() / app::conf.valuta.eur) + " €" );
			list.push_back( text + " = " + QString::number(tmp[0].toFloat() / app::conf.valuta.usd) + " $" );
			m_pStringListModel->setStringList( list );
			return;
		}
	}

	// EXEC PATH
	QString path;
	int i=0;
	while (__environ[i] != NULL) {
		QByteArray ba;
		ba.append( __environ[i] );
		if( ba.left(5) == "PATH=" ){
			auto tmp = ba.split('=');
			if( tmp.size() == 2 ) path = tmp[1];
			break;
		}
		i++;
	}

	QString appText = text;
	bool flag = false;
	if( appText.left(1) == "!" ){
		appText.remove(0,1);
		flag = true;
	}

	for(auto elemPath:path.split(":")){
		QDir dir( elemPath );
		for(auto elem:dir.entryList( QStringList() << appText + "*",QDir::Executable | QDir::Files | QDir::NoDotAndDotDot)){
			if( flag ) elem = "!" + elem;
			//поиск дубликатов
			bool find = false;
			for(auto elemList:list){
				if(elemList == elem){
					find = true;
					break;
				}
			}
			if( !find ) list.push_back( elem );
		}
	}

	m_pStringListModel->setStringList( list );
}

void ExecWindow::slot_returnPressed()
{
	if( this->isVisible() ) this->hide();

	auto text = m_pExecLine->text();
	if( text.left(1) == "/" ){
		if( QDir( text ).exists() or QFile( text ).exists() ) emit signal_start("xdg-open",QStringList()<<text);
		return;
	}

	bool flagSu = false;
	bool flagTerm = false;
	if( text.left(1) == "!" ){
		text.remove(0,1);
		flagSu = true;
	}
	if( text.left(1) == "#" ){
		text.remove(0,1);
		flagTerm = true;
	}

	if( flagTerm ){
		if( flagSu ) text = "sudo " + text;
		emit signal_start("exo-open",QStringList()<<"--launch"<<"TerminalEmulator"<<text);
	}else{
		QStringList tmp = text.split(" ");
		if( flagSu ){
			emit signal_start("gksu",tmp);
		}else{
			QString cmd = tmp[0];
			tmp.pop_front();
			emit signal_start(cmd,tmp);
		}
	}
}

void ExecWindow::resizeMonitor()
{
	auto tmp = app::conf.serialMonitor.split("x");
	if( tmp.size() != 2 ) return;
	this->setFixedSize( tmp[0].toUInt(), tmp[1].toUInt() );
}
