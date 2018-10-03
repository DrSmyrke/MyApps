#!/bin/bash
PACKETS_DIR="/my/www/deb/pool/mypackets"
rm $PACKETS_DIR/*.deb

function buildPkt_all(){
	version=$(cat $1/DEBIAN/control | grep Version | awk '{print $2}');
	fakeroot dpkg-deb --build $1 && mv $1.deb $PACKETS_DIR/$2_$version\_all.deb
}
function buildPkt(){
	version=$(cat $1/DEBIAN/control | grep Version | awk '{print $2}');
	fakeroot dpkg-deb --build $1 && mv $1.deb $PACKETS_DIR/$2_$version\_$3.deb
}
#buildPkt_all <DIR> <PACKET_NAME>
#buildPkt <DIR> <PACKET_NAME> <amd64|i386>


buildPkt_all "myconfigs" "myconfigs"
buildPkt_all "myserverconfigs" "myserverconfigs"
buildPkt_all "myfonts" "myfonts"
buildPkt_all "myicons" "myicons"
buildPkt "myothersoft_x64" "myothersoft" "amd64"
buildPkt_all "myscripts" "myscripts"
buildPkt_all "mythemes" "mythemes"
buildPkt_all "mywallpapers" "mywallpapers"
buildPkt "otherSoft_i386" "othersoft" "i386"
buildPkt "otherSoft_x64" "othersoft" "amd64"
buildPkt "MySoft/mywebproxy_x64" "mywebproxy" "amd64"
buildPkt "MySoft/mypanel_x64" "mypanel" "amd64"

cd /my/www/deb
./make.sh

#for filename in /Data/*.txt; do
#    for ((i=0; i<=3; i++)); do
#        ./MyProgram.exe "$filename" "Logs/$(basename "$filename" .txt)_Log$i.txt"
#    done
#done

