/**
  * @file ip.h
  * @brief Предоставляет способы хранения и работы с IP-адресом
  */

#ifndef IP_H
#define IP_H

#include <cstdint>
#include <string>
#include <utility>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * @brief The IP class
 * Предоставляет способы хранения и работы с IP-адресом
 */
class IP {
public:
    /**
     * Конструктор по умолчанию
     */
    IP() = default ;

    /**
     * Конструктор копирования
     */
    IP( const IP &v ) ;

    /**
     * @brief IP
     * Создаёт экземпляр класса IP, инициализируя его значением из четырёх байт
     * в типе int
     * @param v
     * IP-адрес
     */
    IP( const std::int32_t v ) ;

    /**
     * @brief IP
     * Создаёт экземпляр класса IP, инициализируя его значением из четырёх байт
     * в типе uint
     * @param v
     * IP-адрес
     */
    IP( const std::uint32_t v ) ;

    /**
     * @brief IP
     * Создаёт экземпляр класса IP, инициализируя его значением из четырёх байт
     * по указателю на буфер
     * @param v
     * Указатель на буфер с IP-адресом
     */
    IP( unsigned char *buff ) ;

    /**
     * @brief IP
     * Создаёт экземпляр класса IP, инициализируя его значением из значения
     * типа строки
     * @param v
     * Строка с IP-адресом
     */
    IP( const std::string &v ) ;

    /**
     * @brief IP
     * Создаёт экземпляр класса IP, инициализируя его значением из значения
     * указателя на C-строку
     * @param buff
     * Строка с IP-адресом
     */
    IP( const char *buff ) ;

    /**
     * @brief operator std::string
     * Возвращает текущее значение в виде строки
     */
    operator std::string();

    /**
     * @brief operator int
     * Возвращает текущее значение в виде int
     */
    operator int() const ;

    /**
     * @brief operator []
     * Возвращает байт адреса с заданным индексом
     * @param index
     * Индекс байта
     * @return
     * Байт адреса
     */
    char &operator[] ( int index ) ;

    /**
     * @brief set
     * Устанавливает текущее значение из четырёх байт в типе int
     * @param v
     * Значение IP в виде числа int
     */
    void set( const std::int32_t v ) ;

    /**
     * @brief set
     * Устанавливает текущее значение из четырёх байт в типе uint
     * @param v
     * Значение IP в виде числа uint
     */
    void set( const std::uint32_t v ) ;

    /**
     * @brief set
     * Устанавливает текущее значение из строки
     * @param v
     * Значение IP в строковом формате
     */
    void set( const std::string &v ) ;

    /**
     * @brief toStr
     * Возвращает текущее значение в виде строки
     * @return
     */
    std::string toStr() const noexcept ;

    /**
     * @brief toAscii
     * Возвращает указатель на представление текущего значения в виде C-строки.
     * Для хранения результата используется статический буфер, и, как следствие,
     * каждый вызов этого метода приведёт к изменению данных по указателю на
     * результат в предыдущем вызове.
     * @return
     * Указатель на результирующий буфер
     */
    char *toAscii() const noexcept ;

    /**
     * @brief isValid
     * Проверяет текущее значение на корректность
     * @return
     * true, если текущее значение IP корректно; иначе false
     */
    bool isValid() const noexcept ;

    /**
     * @brief isValid
     * Проверяет указанное значение на корректность
     * @return
     * true, если значение IP корректно; иначе false
     */
    template< typename T > static bool isValid( T &&v ){
        return IP( std::forward< T >( v ) ).isValid() ;
    }

    /**
     * @brief reverse
     * Возвращает новый экземпляр IP с обратным порядком байт
     * @return
     * Новый экземпляр IP с обратным порядком байт
     */
    IP reverse() const noexcept ;

    /**
     * @brief dnsPTR
     * Возвращает IP в виде строки в формате in-addr.arpa
     * @return
     */
    std::string dnsPTR() const ;

private:
    bool m_isValid = false ;
    union {
        std::uint32_t m_ip ;
        char byte[ 4 ] ;
    } ;
};

#endif // IP_H
