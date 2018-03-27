#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include "global.h"

class Settings : public QMainWindow
{
	Q_OBJECT
public:
	explicit Settings(QWidget *parent = 0);
	void open();
private:
	QTextEdit* m_pEditor;
	QTextEdit* m_pSyncSaveDirs;
	QLineEdit* m_pRUser;
	QLineEdit* m_pRDir;
	QLineEdit* m_pPersonalDir;
	QLineEdit* m_pRemoteServer;
	QSpinBox* m_pRemotePort;
	QCheckBox* m_pSyncOnStart;
	QRadioButton* m_pStaticSwapB;
	QRadioButton* m_pDynamicSwapB;
	bool m_changeSettings = false;
};

#endif // SETTINGS_H
