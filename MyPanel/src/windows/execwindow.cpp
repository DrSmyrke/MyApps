#include "execwindow.h"
#include <QLayout>
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
ExecWindow::ExecWindow(QWidget *parent) : QMainWindow(parent)
{
	m_pCompleter = new QCompleter(this);
		m_pCompleter->setModel(new QDirModel(m_pCompleter));
	m_pExecLine = new QLineEdit(this);
		m_pExecLine->setCompleter(m_pCompleter);
	m_pExecB = new QPushButton(this);
		m_pExecB->setIcon( QIcon("://img/apply.png") );

	QWidget* centrWidget = new QWidget(this);
		QHBoxLayout* hBox = new QHBoxLayout();
			hBox->setSpacing(3);
			hBox->setMargin(2);
		hBox->addWidget(m_pExecLine);
		hBox->addWidget(m_pExecB);
	centrWidget->setLayout(hBox);

	setCentralWidget(centrWidget);
	setWindowFlags(Qt::Popup);
	setFixedSize(350,40);

	connect(m_pExecLine,&QLineEdit::textChanged,this,&ExecWindow::slot_textChanged);
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
	//if( text.left(1) == "/" ){

	//	return;
	//}

	//qDebug()<<m_pCompleter->model()->rowCount();

}

void ExecWindow::resizeMonitor()
{
	auto tmp = app::conf.serialMonitor.split("x");
	if( tmp.size() != 2 ) return;
	this->setFixedSize( tmp[0].toUInt(), tmp[1].toUInt() );
}
