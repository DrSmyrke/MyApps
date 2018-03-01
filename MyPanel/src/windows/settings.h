#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include "global.h"

class Settings : public QMainWindow
{
	Q_OBJECT
public:
	explicit Settings(QWidget *parent = 0);
	void open();
private:
	QComboBox* m_pSyncMode;
	QTextEdit* m_pEditor;
	QTextEdit* m_pSyncDirs;
	QTextEdit* m_pSyncSaveDirs;
	QLineEdit* m_pRUser;
	QLineEdit* m_pRDir;
	QLineEdit* m_pRemoteServer;
	QSpinBox* m_pRemotePort;
	bool m_changeSettings = false;
};

#endif // SETTINGS_H
