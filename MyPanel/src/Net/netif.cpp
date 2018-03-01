#include "netif.h"

#include <memory.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

NetIf::NetIf()
    : m_isValid( false )
{}

NetIf::NetIf(const ifaddrs * const iap )
    : m_name( iap->ifa_name )
    , m_ip( ( reinterpret_cast< std::uint32_t >( ( reinterpret_cast< sockaddr_in* >( iap->ifa_addr ) )->sin_addr.s_addr ) ) )
    , m_isValid( true )
{
    ifreq ifr ;
    strcpy( ifr.ifr_name, m_name.c_str() ) ;

    auto sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_IP ) ;

    // Получаем MAC-адрес интерфейса
    ioctl( sock, SIOCGIFHWADDR, &ifr ) ;
    m_mac = MAC( &ifr ) ;

    // Получаем MTU
    ioctl( sock, SIOCGIFMTU, &ifr );
    m_mtu = ifr.ifr_mtu ;

    close(sock);
}

NetIfList NetIf::list()
{
    NetIfList res ;
    list( res ) ;
    return res ;
}

void NetIf::list(NetIfList &res)
{
    res.clear() ;
    find( [ &res ]( ifaddrs *iap ) -> bool {
        res.emplace_back( iap ) ;
        return true ;
    }) ;
}

NetIf NetIf::byName(const std::string &name)
{
    return find( [ &name ]( ifaddrs *iap ) -> bool {
        if ( name == iap->ifa_name ){
            return false ;
        }
        return true ;
    }) ;
}

NetIf NetIf::byIP(const IP &addr)
{
    return find( [ &addr ]( ifaddrs *iap ) -> bool {
        if ( addr == ( static_cast< std::int32_t >( ( reinterpret_cast< sockaddr_in* >( iap->ifa_addr ) )->sin_addr.s_addr ) ) ){
            return false ;
        }
        return true ;
    }) ;
}

NetIf NetIf::byRouteTo(const IP &addr)
{
    int dns_port = 53;

    struct sockaddr_in serv;

    int sock = socket ( AF_INET, SOCK_DGRAM, 0);

    memset( &serv, 0, sizeof(serv) );
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr( addr.toStr().c_str() );
    serv.sin_port = htons( dns_port );

    connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );

    sockaddr_in name;
    socklen_t namelen = sizeof( name ) ;
    getsockname( sock, reinterpret_cast< sockaddr* >( &name ), &namelen ) ;
    close( sock ) ;

    return NetIf::byIP( IP( name.sin_addr.s_addr ) ) ;
}

std::string NetIf::name() const noexcept
{
    return m_name;
}

IP NetIf::ip() const noexcept
{
    return m_ip ;
}

MAC NetIf::mac() const noexcept
{
    return m_mac ;
}

int NetIf::mtu() const noexcept
{
    return m_mtu ;
}

bool NetIf::isValid() const noexcept
{
    return m_isValid;
}
