#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QX11Info>

class NativeEventFilter : public QObject, public QAbstractNativeEventFilter
{
	Q_OBJECT
public:
	enum{
		key_mode_ctrl = 0x01,
		key_mode_lalt,
		key_mode_rwin,
		key_type_R = 0x0052,
		key_type_E = 0x0045,
		key_type_Tilda = 0x007e,
	};
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
