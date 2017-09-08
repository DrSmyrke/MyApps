#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QLabel>
#include "widgets/imageeditor.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(const char *filename, QWidget *parent = 0);
	~MainWindow();
private:
	ImageEditor* m_pEditor;
	QSpinBox* m_pCropWidthBox;
	QSpinBox* m_pCropHeightBox;
	QLabel* m_pFilePath;
	QLabel* m_pFileSize;

	QString m_fileName;
};

#endif // MAINWINDOW_H
