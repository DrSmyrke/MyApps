#include "bookmarkswindow.h"
#include <QLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>

BookmarksWindow::BookmarksWindow(QWidget *parent) : QMainWindow(parent)
{
	m_pTable = new QTableWidget(this);
		m_pTable->setColumnCount(4);
		m_pTable->setShowGrid(true);
		m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pTable->setColumnWidth(2,50);
		//m_pTable->horizontalHeader()->setStretchLastSection(true);
		m_pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		m_pTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
		//m_pTable->hideColumn(0);

	QScrollArea* scrollArea = new QScrollArea(this);
			QVBoxLayout* vBoxSA = new QVBoxLayout();
				vBoxSA->setMargin(2);
			vBoxSA->addWidget(m_pTable);
		scrollArea->setLayout(vBoxSA);

	QPushButton* addB = new QPushButton(tr("Add Row"),this);
	QPushButton* saveB = new QPushButton(tr("SAVE"),this);

	QWidget* centrWidget = new QWidget(this);
		QVBoxLayout* vBox = new QVBoxLayout();
			//hBox->setSpacing(3);
			vBox->setMargin(2);
		vBox->addWidget(scrollArea);
			QHBoxLayout* hBox = new QHBoxLayout();
			hBox->addWidget(new QLabel(""));
			hBox->addWidget(addB);
			hBox->addWidget(saveB);
			hBox->setStretch(0,1);
		vBox->addLayout(hBox);
	centrWidget->setLayout(vBox);

	setCentralWidget(centrWidget);
	//setWindowFlags(Qt::Popup);
	setFixedSize(500,240);
	setWindowTitle( tr("Bookmarks") );

	connect(saveB,&QPushButton::clicked,this,&BookmarksWindow::slot_save);
	connect(addB,&QPushButton::clicked,this,[this](){ addRow("","",false,"",0); });
}

void BookmarksWindow::open()
{
	if( this->isHidden() ) this->show();
	m_pTable->clear();
	m_pTable->setHorizontalHeaderLabels(QStringList()<<tr("Name")<<tr("Path")<<tr("Mount")<<tr("Mount point"));
	m_pTable->setRowCount( 0 );
	addRow("","");

	for(auto elem:app::conf.bookmarks) addRow(elem.name, elem.path, elem.mount, elem.mountDir);
}

void BookmarksWindow::slot_save()
{
	this->hide();

	app::conf.bookmarks.clear();
	for(int i = 0; i < m_pTable->rowCount(); i++){
		Bookmark bm;
		bm.name = m_pTable->item(i,0)->text();
		bm.path = m_pTable->item(i,1)->text();
		bm.mount = ( m_pTable->item(i,2)->checkState() == Qt::Checked )?true:false;
		bm.mountDir = m_pTable->item(i,3)->text();
		if( !bm.name.isEmpty() and !bm.path.isEmpty() ) app::conf.bookmarks.push_back( bm );
	}

	app::saveSettings();

	emit signal_saveBookmarks();
}

void BookmarksWindow::addRow(const QString &name, const QString &path, bool mount, const QString &mountDir, const int rowNum)
{
	int newRowNum = ( rowNum != -1 )?rowNum:m_pTable->rowCount();
	m_pTable->insertRow( newRowNum );
	m_pTable->setItem(newRowNum,0, new QTableWidgetItem( name ));
	m_pTable->setItem(newRowNum,1, new QTableWidgetItem( path ));

	QTableWidgetItem *item = new QTableWidgetItem();
	item->data(Qt::CheckStateRole);
	if( mount ){
		item->setCheckState(Qt::Checked);
	} else {
		item->setCheckState(Qt::Unchecked);
	}
	m_pTable->setItem(newRowNum,2, item);

	m_pTable->setItem(newRowNum,3, new QTableWidgetItem( mountDir ));

	//m_pTable->resizeColumnsToContents();
}

