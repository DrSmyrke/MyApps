#!/bin/bash
DIR="/www/deb/pool/mysoft"

rm $DIR/*.deb

function buildPkt_all(){
	version=$(cat $1/DEBIAN/control | grep Version | awk '{print $2}');
	fakeroot dpkg-deb --build $1 && mv $1.deb $DIR/$2_$version\_all.deb
}
function buildPkt(){
	version=$(cat $1/DEBIAN/control | grep Version | awk '{print $2}');
	fakeroot dpkg-deb --build $1 && mv $1.deb $DIR/$2_$version\_$3.deb
}
#buildPkt_all <DIR> <PACKET_NAME>
#buildPkt <DIR> <PACKET_NAME> <amd64|i386>



buildPkt_all "myconfigs" "myconfigs"
#buildPkt_all "myfonts" "myfonts"
#buildPkt_all "myicons" "myicons"
#buildPkt "myothersoft_x64" "myothersoft" "amd64"
#buildPkt_all "myscripts" "myscripts"
#buildPkt "mysoft_i386" "mysoft" "i386"
#buildPkt "mysoft_x64" "mysoft" "amd64"
#buildPkt_all "mythemes" "mythemes"
#buildPkt_all "mywallpapers" "mywallpapers"
#buildPkt "otherSoft_i386" "othersoft" "i386"
#buildPkt "otherSoft_x64" "othersoft" "amd64"

cd /www/deb
./buildRepo.sh
