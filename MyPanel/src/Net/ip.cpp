#include "ip.h"
//#include "services.h"

#include <string.h>

/**
 * @brief IP::IP
 * Конструктор копирования
 * @param v
 * Ссылка на экземпляр класса IP
 */
IP::IP(const IP &v)
{
    m_ip = v.m_ip ;
    m_isValid = v.m_isValid ;
}

IP::IP(const std::int32_t v)
{
    set( v ) ;
}

IP::IP(const std::uint32_t v)
{
    set( v ) ;
}

IP::IP(unsigned char *buff)
    : IP( *reinterpret_cast<std::int32_t*>( buff ) )
{}

IP::IP(const std::string &v)
{
    set( v ) ;
}

IP::IP(const char *buff)
    : IP( std::string( buff ) )
{}

char &IP::operator[](int index)
{
    return byte[ index ] ;
}

bool IP::isValid() const noexcept
{
    return m_isValid ;
}

IP IP::reverse() const noexcept
{
    IP r( *this ) ;
    std::swap( r.byte[ 0 ], r.byte[ 3 ] ) ;
    std::swap( r.byte[ 1 ], r.byte[ 2 ] ) ;
    return r ;
}

std::string IP::dnsPTR() const
{
    return reverse().toStr() + ".in-addr.arpa" ;
}

void IP::set(const std::int32_t v)
{
    set( static_cast< std::uint32_t >( v ) ) ;
}

void IP::set(const std::uint32_t v)
{
    m_ip = v ;
    m_isValid = true ;
}

void IP::set(const std::string &v)
{
    in_addr t_addr ;
    if ( inet_pton( AF_INET, v.c_str(), &t_addr ) == 1 ) {
        m_ip = t_addr.s_addr ;
        m_isValid = true ;
    } else {
        m_isValid = false ;
    }
}

std::string IP::toStr() const noexcept
{
    return std::string( toAscii() ) ;
}

char *IP::toAscii() const noexcept
{
    static char buff[ 20 ] ;
    if ( ! m_isValid ) {
        strcpy( buff, "[Undefined]" ) ;
    } else {
        in_addr ip_addr;
        ip_addr.s_addr = m_ip;
        strcpy( buff, inet_ntoa( ip_addr ) ) ;
    }
    return buff ;
}

IP::operator int() const
{
    return m_ip ;
}

IP::operator std::string()
{
    return toStr() ;
}
