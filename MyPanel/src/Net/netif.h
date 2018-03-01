/**
  * @file netif.h
  * @brief Предоставляет средства для работы с интерфейсами
  */

#ifndef NETIF_H
#define NETIF_H

#include <vector>
#include <string>

#include <net/if.h>
#include <ifaddrs.h>

#include "Net/ip.h"
#include "Net/mac.h"

class NetIf ;

/**
 * @brief NetIfList
 * Синоним для вектора объектов NetIf
 */
using NetIfList = std::vector< NetIf > ;

/**
 * @brief The NetIf class
 * Предоставляет средства для работы с интерфейсами
 */
class NetIf
{
public:
    /**
     * @brief NetIf
     * Создаёт невалидный экземпляр класса NetIf
     */
    NetIf() ;

    /**
     * @brief NetIf
     * Создаёт экземпляр класса NetIf, инициализируемой структорой iap
     * @param iap
     * Структура для инициализации типа ifaddr
     */
    NetIf( const ifaddrs *const iap ) ;

    /**
     * @brief list
     * Возвращает список существующих и поднятых интерфейсов
     * @return Список интерфейсов
     */
    static NetIfList list() ;

    /**
     * @brief list
     * Получает список существующих и поднятых интерфейсов
     * @param res
     * Ссылка на список интерфесов
     */
    static void list( NetIfList &res ) ;

    /**
     * @brief byName
     * Возвращает объект типа NetIf по имени интерфейса
     * @param name
     * Имя интерфейса
     * @return
     * Заполненный объект типа NetIf, если интерфейс с указанным именем найден;
     * иначе невалидный объект NetIf
     */
    static NetIf byName( const std::string &name ) ;

    /**
     * @brief byIP
     * Возвращает объект типа NetIf по его IP-адресу
     * @param ip
     * IP-адрес интерфейса
     * @return
     * Заполненный объект типа NetIf, если интерфейс с указанным адресом найден;
     * иначе невалидный объект NetIf
     */
    static NetIf byIP( const IP &ip ) ;

    /**
     * @brief byRouteTo
     * Возвращает объект типа NetIf, определяющий интерфейс, через который будет
     * устанавливаться соединение с заданным адресом
     * @param ip
     * IP-адрес удалённой стороны, с которой будет устанавливаться соединение
     * @return
     * Заполненный объект типа NetIf
     */
    static NetIf byRouteTo( const IP &ip ) ;

    /**
     * @brief isValid
     * Проверяет объект на валидность
     * @return
     * true, если объект валиден; иначе false
     */
    bool isValid() const noexcept ;

    /**
     * @brief name
     * Возвращает имя интерфейса
     * @return
     * Имя интерфейса
     */
    std::string name() const noexcept ;

    /**
     * @brief ip
     * Возвращает IP-адрес интерфейса
     * @return
     * IP-адрес интерфейса
     */
    IP ip() const noexcept ;

    /**
     * @brief mac
     * Возвращает MAC-адрес интерфейса
     * @return
     * MAC-адрес
     */
    MAC mac() const noexcept ;

    /**
     * @brief mtu
     * Возвращет MTU интерфейса
     * @return
     * MTU интерфейса
     */
    int mtu() const noexcept ;

private:
    std::string m_name = "[Undefined]" ;
    IP m_ip ;
    MAC m_mac ;
    int m_mtu ;
    bool m_isValid = false ;

    // Перебирает интерфейсы, передавай функтору f указатель на структуру
    // типа ifaddrs для каждого из них до тех пор, пока функтор не вернёт false
    // или интерфейсы не закончатся
    template< typename T > static inline NetIf find( T f ) {
        NetIf res ;
        ifaddrs *addrs ;

        getifaddrs( &addrs ) ;
        for ( auto iap = addrs; iap != NULL; iap = iap->ifa_next ) {
            if ( iap->ifa_addr && ( iap->ifa_flags & IFF_UP ) && ( iap->ifa_addr->sa_family == AF_INET) && ( ! f( iap ) ) ) {
                res = NetIf( iap ) ;
                break ;
            }
        }
        freeifaddrs(addrs);
        return res ;
    }
};

#endif // NETIF_H
