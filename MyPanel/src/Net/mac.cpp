#include "mac.h"

#include <memory.h>
#include <stdio.h>

MAC::MAC(const MAC &o)
{
    memcpy( m_data, o.m_data, 6 ) ;
    m_isValid = o.isValid() ;
}

MAC::MAC( const ifreq *const ifr )
    : MAC( ifr->ifr_hwaddr.sa_data )
{}

MAC::MAC( const char *buff )
{
    memcpy( m_data, buff, 6  ) ;
    m_isValid = true ;
}

bool MAC::isValid() const
{
    return m_isValid;
}

std::string MAC::toStr() const
{
    if ( ! m_isValid ) return "[Undefined]" ;

    char res[ 24 ] ;
    sprintf( res, "%02x:%02x:%02x:%02x:%02x:%02x",
        (unsigned char)m_data[0],
        (unsigned char)m_data[1],
        (unsigned char)m_data[2],
        (unsigned char)m_data[3],
        (unsigned char)m_data[4],
        (unsigned char)m_data[5] ) ;
    return std::string( res ) ;
}

MAC &MAC::operator=( const MAC &o )
{
    memcpy( m_data, o.m_data, 6 ) ;
    m_isValid = o.isValid() ;
    return *this ;
}

MAC::operator std::string()
{
    return toStr() ;
}
