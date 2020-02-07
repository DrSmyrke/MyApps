#include "nativeeventfilter.h"
#include <QVector>
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace
{
	Window m_win;               // Захватываемое окно - в данном случае будет вся система
	Display* m_pDisplay;        // Соединение с сервером X11

	std::vector<uint32_t> maskModifiers()
	{
		std::vector<uint32_t> masks;
		masks.push_back( 0 );
		masks.push_back( Mod2Mask );
		masks.push_back( LockMask );
		masks.push_back( (Mod2Mask | LockMask) );
		return masks;
	}
}

NativeEventFilter::NativeEventFilter(QObject *parent) : QObject(parent)
{
	m_pDisplay = QX11Info::display();        // Создадим подключение к серверу
	m_win = DefaultRootWindow(m_pDisplay);   // и вытащим из него захватываемое окно с помощью макроса
	m_keys[ XKeysymToKeycode(m_pDisplay, XK_E) ] = ControlMask;
	m_keys[ XKeysymToKeycode(m_pDisplay, XK_asciitilde) ] = ControlMask;
}

bool NativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
	Q_UNUSED(eventType)
	Q_UNUSED(result)
	/* В вот обработка события строится уже на библиотеке XCB вместо Xlib.
	 * Вроде как, получая событие Qt знает его в качестве XCB события,
	 * но не знает его в качестве события Xlib, хотя использовать более
	 * легкий синтаксис Xlib для установки хоткеев нам никто не запрещает
	 * */
	xcb_key_press_event_t *keyEvent = 0;

	// Итак проверяем, что это было xcb событие
	if (eventType == "xcb_generic_event_t") {
		// кастуем сообщение собственно в xcb событие
		xcb_generic_event_t *event = static_cast<xcb_generic_event_t *>(message);

		// проверяем, что произошло нажатие клавиши
		if ((event->response_type & 127) == XCB_KEY_PRESS){
			// Если так, то кастуем сообщение в событие нажатия клавиши
			keyEvent = static_cast<xcb_key_press_event_t *>(message);
			// Далее проверям, является ли это событие нужным хоткее
			for(auto key:m_keys){
				for(auto mask:maskModifiers()){
					if((keyEvent->state == (key.second | mask )) &&  keyEvent->detail == key.first){
						uint8_t mode = 0;
						switch (key.second) {
							case ControlMask: mode = key_mode_ctrl; break;
							case Mod1Mask: mode = key_mode_lalt; break;
							case Mod4Mask: mode = key_mode_rwin; break;
						}
						auto keySym = XKeycodeToKeysym(m_pDisplay,key.first,1);
						emit activated(mode,keySym);
						return true;
					}
				}
			}
		}
	}
	return false;
}
void NativeEventFilter::setShortcut()
{
	unsetShortcut();
	/* А теперь пройдемся по всем возможным комбинациям с учётом Num Lock и Caps Lock
	 * устанавливая хоткеи
	 * */
	for(auto key:m_keys){
		for(auto mask:maskModifiers()){
			XGrabKey(m_pDisplay,key.first,key.second | mask, m_win, True, GrabModeAsync, GrabModeAsync);
		}
	}
}

void NativeEventFilter::unsetShortcut()
{
	// Проходим по всем возможным комбинациям и удаляем хоткей
	for(auto key:m_keys){
		for(auto mask:maskModifiers()){
			XUngrabKey(m_pDisplay,key.first,key.second | mask, m_win);
		}
	}
}
