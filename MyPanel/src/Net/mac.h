/**
  * @file mac.h
  * @brief Хранит и предоставляет методы для работы с MAC-адресом
  */
#ifndef MAC_H
#define MAC_H

#include <string>

#include <net/if.h>

/**
 * @brief The MAC class
 * Хранит и предоставляет методы для работы с MAC-адресом
 */
class MAC
{
public:
    /**
      * @brief MAC
      * Конструктор по умолчанию
      */
    MAC() = default ;

    /**
     * @brief MAC
     * Конструктор копирования
     * @param o
     * Ссылка на копируемый экземпляр MAC
     */
    MAC( const MAC &o ) ;

    /**
     * @brief MAC
     * Конструктор
     * @param ifr
     * Структура типа ifreq, из которой будет извлечён MAC-адрес
     */
    MAC(const ifreq * const ifr ) ;

    /**
     * @brief MAC
     * Конструктор
     * @param buff
     * Буфер, содержащий 6 байт MAC-адреса
     */
    MAC(const char *buff ) ;

    /**
     * @brief isValid
     * Проверяет экземпляр на корректность
     * @return
     * true, если MAC-адрес корректен; иначе false
     */
    bool isValid() const;

    /**
     * @brief toStr
     * Возвращает строкове представление MAC-адреса
     * @return
     * Строка с MAC-адресом
     */
    std::string toStr() const ;

    /**
     * @brief operator std::string
     * Возвращает строкове представление MAC-адреса
     */
    operator std::string() ;

    /**
     * @brief operator =
     * Оператор присваивания объектов MAC
     * @param o
     * Правый аргумент оператора присваивания
     * @return
     * Ссылка на левый аргумент оператора присваивания
     */
    MAC &operator=( const MAC& o ) ;

private:
    bool m_isValid = false ;
    char m_data[ 6 ] ;
};

#endif // MAC_H
