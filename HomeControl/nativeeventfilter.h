#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include "global.h"

class NativeEventFilter : public QObject, public QAbstractNativeEventFilter
{
	Q_OBJECT
public:
	explicit NativeEventFilter(QObject *parent = 0);
	// переопределяем метод nativeEventFilter
	bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
	void setShortcut();     // Добавляем метод установки хоткея
	void unsetShortcut();   // и метод удаления хоткея для полноты картины
signals:
	void activated(const uint8_t mode, const uint16_t key);
private:
	std::map<int,unsigned int> m_keys;
};

#endif // NATIVEEVENTFILTER_H
