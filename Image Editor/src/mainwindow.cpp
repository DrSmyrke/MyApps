#include "mainwindow.h"
#include <QLayout>
#include <QPushButton>
#include <QImage>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(const char *filename, QWidget *parent)
	: QMainWindow(parent)
	, m_fileName(filename)
{
	m_pEditor = new ImageEditor(this);

	QPushButton* cropB=new QPushButton(tr("Crop"),this);
		connect(cropB,&QPushButton::clicked,this,[this](){
			//QPixmap newImg=QPixmap::fromImage(img,Qt::NoFormatConversion);
			m_pEditor->getImage().save(m_fileName,0,100);
		});
	m_pFilePath=new QLabel(m_fileName,this);
	m_pFileSize=new QLabel();
	m_pCropWidthBox = new QSpinBox(this);
		m_pCropWidthBox->setRange(1,30000);
		m_pCropWidthBox->setValue(600);
		connect(m_pCropWidthBox,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,[this](int val){
			m_pEditor->setNewSize(QSize(val,m_pCropHeightBox->value()));
		});
	m_pCropHeightBox = new QSpinBox(this);
		m_pCropHeightBox->setRange(1,30000);
		m_pCropHeightBox->setValue(400);
		connect(m_pCropHeightBox,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,[this](int val){
			m_pEditor->setNewSize(QSize(m_pCropWidthBox->value(),val));
		});

	setWindowTitle("Image editor v0.1");
	setMinimumSize(640,480);

	QWidget* centrWidget=new QWidget(this);
		QVBoxLayout* vBox=new QVBoxLayout();
			QHBoxLayout* filePanel=new QHBoxLayout();
				filePanel->addWidget(new QLabel(tr("File:")));
				filePanel->addWidget(m_pFilePath,1);
				filePanel->addWidget(m_pFileSize);
			vBox->addLayout(filePanel);
			QHBoxLayout* cropPanel=new QHBoxLayout();
				cropPanel->addWidget(new QLabel(tr("Crop image:")));
				cropPanel->addWidget(m_pCropWidthBox,1);
				cropPanel->addWidget(new QLabel("X"));
				cropPanel->addWidget(m_pCropHeightBox,1);
				cropPanel->addWidget(cropB);
			vBox->addLayout(cropPanel);
			vBox->addWidget(m_pEditor,1);
		centrWidget->setLayout(vBox);
	setCentralWidget(centrWidget);



	QTimer::singleShot(100,this,[this](){
		m_pEditor->setImage(m_fileName);
		m_pEditor->setNewSize(QSize(m_pCropWidthBox->value(),m_pCropHeightBox->value()));
		QSize sz=m_pEditor->getImageSize();
		m_pFileSize->setText(QString::number(sz.width())+"x"+QString::number(sz.height()));
	});

}

MainWindow::~MainWindow()
{

}
