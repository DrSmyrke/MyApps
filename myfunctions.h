#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H
#include <QDir>

namespace mf {
	void removeDir(const QString &path);
	QString getSize(const long val);
	QByteArray toBigEndianInt(const int val);
	QByteArray toBigEndianShort(const short val);
	unsigned int getRand(const int max);
	/**
	 * @brief md5
	 * Get md5 summ for string
	 * @param string
	 * @return
	 */
	QByteArray md5(const QByteArray &string);
	bool startDetached(const QString &cmd, const QStringList &args);
	void setLog(const uint8_t logLevel, const QString &mess);
}

#endif // MYFUNCTIONS_H
