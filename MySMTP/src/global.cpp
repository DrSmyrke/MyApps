#include "global.h"

namespace app {
	SMTP_Config smtpConf;
}
namespace smtp {

	SMTP_Pkt parsPkt(const QByteArray &data)
	{
		SMTP_Pkt pkt;

		for(uint32_t i = 0; i < data.size(); i++){
			if( pkt.cmd.size() == 0 and data.at(i) == ' ' ){
				pkt.cmd.append( pkt.data );
				pkt.data.clear();
				continue;
			}
			pkt.data.append( data[i] );
		}

		if( pkt.cmd.size() > 0 and pkt.data.right( 2 ) == "\r\n" ) pkt.valid = true;

		return pkt;
	}

}
SMTP SERVER at 2525 port STARTED
"EHLO" "forward24o.cmail.yandex.net\r\n"
"MAIL" "FROM:<smyrke2005@ya.ru>\r\n"
^CДля закрытия данного окна нажмите <ВВОД>...
