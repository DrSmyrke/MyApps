#!/bin/bash
# based on DrSmyrke
#
# Any original DrSmyrke code is licensed under the BSD license
#
# All code written since the fork of DrSmyrke is licensed under the GPL
#
#
# Copyright (c) 2016 Prokofiev Y. <Smyrke2005@yandex.ru>
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#
if ! [ $1 ];then exit;fi
if [ $1 = "-h" ];then
	echo "Usage:	$0 <SCRIPT> ARGUMENTS"
	echo "Scripts:"
	echo "s1 - Current video driver"
	echo "s2 - View progress at dd"
	echo "s3 - Clear swap"
	echo "s4 - Your IP"
	echo "s5 - Ping wour LAN"
	echo "s6 - PskovLineBallance <login> <password>"
	echo "s7 - Streaming WebCam to Lan (http://localhost:port/1.ogv) <port>"
	echo "s8 - Add current user to group vboxusers"
	echo "s9 - Maximum energy"
	echo "s10 - Face control"
	echo "s11 - makeCasperRW <size> (default 280Mb)"
	echo "s12 - backup ~/bin with SSH server <login> <server> <remotedir>"
	echo "s13 - convert man page to pdf <command>"
	echo "s14 - view 1-100 percents"
	echo "s15 - check port open <host> <port>"
	echo "s16 - Password recovery root"
	echo "s17 - GRUB recovery"
	echo "s18 - decode HEX to UTF-8 <string>"
	echo "s19 - get USD,EUR value"
	echo "s20 - unlock USB GSM modem <unlock code> <device> #/dev/ttyUSB{0,1,2} #unlock code get http://a-zgsm.com/freecode/"
	echo "s21 - pstree"
	echo "s22 - screenCast"
	echo "s23 - screenCast NOSOUND"
	echo "s24 - create system default X-Cursor file"
	echo "s25 - write ISO image to USB flash"
	echo "s26 - CPU usage"
	echo "s27 - BASH colormap"
	echo "s28 - DISTRIBUTIVE info"
	echo "s29 - LED ScrollLock on/off"
	echo "s30 - Echo IP to name"
	echo "s31 - Enter password"
	echo "s32 - Crypt ~/passwd"
	echo "s33 - Current day"
    echo "s34 - Install QT DEV"
fi
if [ $1 = "s1" ];then
	grep -Eiwo -m1 'nvidia|amd|ati|intel' /var/log/Xorg.0.log
fi
if [ $1 = "s2" ];then
	watch -n 5 sudo killall -USR1 dd
fi
if [ $1 = "s3" ];then
	sudo swapoff -a
	sudo swapon -a
fi
if [ $1 = "s4" ];then
	MYIP=$(wget -O - -q icanhazip.com);
	echo $MYIP;
fi
if [ $1 = "s5" ];then
	echo -n "Enter IP range (192.168.1):"
	read range
	if ! [ $range ];then range="192.168.1";fi
	echo -n "Enter $range."
	read lsubrange
	if ! [ $lsubrange ];then lsubrange="0";fi
	echo -n "Enter IP subrange high $range.$lsubrange-"
	read hsubrange
	if ! [ $hsubrange ];then hsubrange="255";fi
	for ((i=$lsubrange; i<=$hsubrange; i++))
	do
		status=$(ping "$range.$i" -c 1 2>/dev/null | grep "packet loss" | awk '{print $6}' | awk -F'%' '{print $1}');
		if [ "$status" = "0" ]; then echo -e "\e[1;32m$range.$i	OnLine\e[0m";fi
		if [ "$status" = "+1" ]; then echo -e "\e[1;31m$range.$i	OffLine\e[0m";fi
	done
fi
if [ $1 = "s6" ];then
	str=$(wget -q --post-data "login=$2&password=$3" https://stat.pskovline.ru/ --no-check-certificate -O /tmp/tmp && cat /tmp/tmp | grep 'style="font-size:15px">');
	string=$(echo $str | awk '{print $1}' | awk -F'>' '{print $2}' | awk -F'<' '{print $1}');
	if [ "$string" = "" ]; then
		string=$(echo $str | awk '{print $2}' | awk -F'>' '{print $2}' | awk -F'<' '{print $1}');
	fi
	echo $string
	rm /tmp/tmp
fi
if [ $1 = "s7" ];then
	vlc -I dummy screen:// --sout "#transcode{vcodec=theo,vb=1200,scale=0,acodec=vorb,ab=128,fps=24}:standard{access=http,mux=ogg,dst=:$port/1.ogv}"
fi
if [ $1 = "s8" ];then
	sudo usermod -a -G vboxusers $USER
fi
if [ $1 = "s9" ];then
	sudo service bluetooth stop
	sudo service networking stop
	sudo service network-interface stop
	sudo service network-manager stop
	sudo service network-interface-security stop
	sudo service avahi-daemon stop
	sudo service apparmor stop
	sudo service cups stop
	sudo service cron stop
fi
if [ $1 = "s10" ];then
	if ! [ -d ~/.facecontrol ]; then
		mkdir ~/.facecontrol
	fi
	data=$(date +\%Y-\%m-\%d-\%H-\%M-\%S);
	ffmpeg -f video4linux2 -s 640x480 -i /dev/video0 -f image2 ~/.facecontrol/$data.jpg
fi
if [ $1 = "s11" ];then
	size=280
	if [ $2 ];then size=$2;fi
	sudo dd if=/dev/zero of=casper-rw bs=1M count=$size
	sudo mkfs.ext4 casper-rw
	#sudo mount casper-rw /tmp/temp -o loop
fi
if [ $1 = "s12" ];then
	7z a -t7z -mx=3 ~/bin.7z ~/bin
	scp ~/bin.7z "$2@$3:$4/bin.7z"
	rm ~/bin.7z
	#sudo su -c "echo '$3' | sshfs -p 22 $2@$4:$5 /mnt/TMPMOUNT -o password_stdin" $luser
fi
if [ $1 = "s13" ];then
	man -t $2 | ps2pdf - $2.pdf
fi
if [ $1 = "s14" ];then
	echo -n "percents -    "; for i in {1..100}; do sleep 0.1; echo -ne '\e[4D'; printf "% 3d%%" $i; done; echo
fi
if [ $1 = "s15" ];then
	if [[ "$(echo "quit" | telnet $2 $3 2>/dev/null | grep 'Escape\ character' | wc -l)" -eq "1" ]]; then echo "OPEN"; else echo "CLOSED"; fi
fi
if [ $1 = "s16" ];then
	sudo fdisk -l
	echo "Enter the name of the partition (example: sda1):"
	read razd
	sudo mount /dev/$razd /mnt
	sudo chroot /mnt
	sudo passwd root
	exit
	sudo umount /mnt
fi
if [ $1 = "s17" ];then
	sudo fdisk -l
	echo "Enter the name of the partition (example: sda1):"
	read razd
	sudo mount /dev/$razd /mnt
	sudo grub-install --root-directory=/mnt /dev/$razd
	sudo update-grub --output=/mnt/boot/grub/grub.cfg
	sudo umount /mnt
fi
if [ $1 = "s18" ];then
	len=$(echo $2 | awk '{print length}' || echo -n $2 | wc -c || expr length $2);
	echo "===================="
	for ((i=0; i<$len; i+=2)); do
		echo -e -n "\x"${2:$i:2}
	done
	echo ""
fi
if [ $1 = "s19" ];then
	dat=$(date +%d/%m/%Y);
	wget -q http://www.cbr.ru/scripts/XML_daily.asp?date_req=$dat -O /tmp/tmp
	usd=$(sed -n '/<CharCode>USD<\/CharCode>/{n;n;n;p;}' /tmp/tmp);
	eur=$(sed -n '/<CharCode>EUR<\/CharCode>/{n;n;n;p;}' /tmp/tmp);
	usd=$(echo $usd | awk '{print $0}' | awk -F'>' '{print $2}' | awk -F'<' '{print $1}');
	eur=$(echo $eur | awk '{print $0}' | awk -F'>' '{print $2}' | awk -F'<' '{print $1}');
	echo "$usd"
	echo "$eur"
	rm /tmp/tmp
fi
if [ $1 = "s20" ];then
	sudo su -c "echo -e 'AT^CARDLOCK=$2\r\n' >$3"
	#PS-1: Еще насколько полезных команд:
	#AT^U2DIAG=0 (режим "только модем")
	#AT^U2DIAG=1 (режим "модем + CD-ROM")
	#AT^U2DIAG=255 (режим "модем + CD-ROM + Card Reader")
	#AT^U2DIAG=256 (режим "модем + Card Reader")
fi
if [ $1 = "s21" ];then
	ps fax
fi
if [ $1 = "s22" ];then
	#vlc -I dummy screen:// --sout "#transcode{vcodec=theo,vb=1200,scale=0,acodec=vorb,ab=128,fps=24}:standard{access=http,mux=ogg,dst=:$port/1.ogv}"
	#pacmd list-sources | grep "name:"
	cvlc screen:// --screen-fps=25.000000 --input-slave=pulse://alsa_input.pci-0000_00_1b.0.analog-stereo --live-caching=300 --sout "#transcode{vcodec=h264,vb=7000,scale=1,acodec=mpga,ab=128,channels=2,samplerate=44100}:file{dst=$HOME/MyScreenCast.mp4}"
fi
if [ $1 = "s23" ];then
	cvlc screen:// --screen-fps=25.000000 --live-caching=300 --sout "#transcode{vcodec=h264,vb=5000,scale=1}:file{dst=$HOME/MyScreenCast.mp4}"
fi
if [ $1 = "s24" ];then
	sudo su -c 'echo "[Icon Theme]" > /usr/share/icons/default/index.theme'
	sudo su -c 'echo "Inherits=pixelfun3" >> /usr/share/icons/default/index.theme'
	sudo su -c 'echo "[Icon Theme]" > /etc/alternatives/x-cursor-theme'
	sudo su -c 'echo "Inherits=pixelfun3" >> /etc/alternatives/x-cursor-theme'
fi
if [ $1 = "s25" ];then
	sudo mintstick -m iso
fi
if [ $1 = "s26" ];then
PREV_TOTAL=0
PREV_IDLE=0
while true; do
	CPU=(`cat /proc/stat | grep '^cpu '`) # Get the total CPU statistics.
	unset CPU[0]                          # Discard the "cpu" prefix.
	IDLE=${CPU[4]}                        # Get the idle CPU time.
	# Calculate the total CPU time.
	TOTAL=0
	for VALUE in "${CPU[@]}"; do
		let "TOTAL=$TOTAL+$VALUE"
	done
	# Calculate the CPU usage since we last checked.
	let "DIFF_IDLE=$IDLE-$PREV_IDLE"
	let "DIFF_TOTAL=$TOTAL-$PREV_TOTAL"
	let "DIFF_USAGE=(1000*($DIFF_TOTAL-$DIFF_IDLE)/$DIFF_TOTAL+5)/10"
	echo -en "\rCPU: $DIFF_USAGE%  \b\b"
	# Remember the total and idle CPU times for the next check.
	PREV_TOTAL="$TOTAL"
	PREV_IDLE="$IDLE"
	# Wait before checking again.
	sleep 1
done
fi
if [ $1 = "s27" ];then
	T='gYw'   # The test text
	echo -e "\n                 40m     41m     42m     43m\
     44m     45m     46m     47m";
	for FGs in '    m' '   1m' '  30m' '1;30m' '  31m' '1;31m' '  32m' \
           '1;32m' '  33m' '1;33m' '  34m' '1;34m' '  35m' '1;35m' \
           '  36m' '1;36m' '  37m' '1;37m';
	do FG=${FGs// /}
	echo -en " $FGs \033[$FG  $T  "
	for BG in 40m 41m 42m 43m 44m 45m 46m 47m;
		do echo -en "$EINS \033[$FG\033[$BG  $T  \033[0m";
	done
	echo;
	done
	echo
fi
if [ $1 = "s28" ];then
	cat /proc/version
	lsb_release -a
	cat /etc/*-release
fi
if [ $1 = "s29" ];then
	for i in {1..100}; do xset led named "Scroll Lock" ;sleep 0.05 ;xset -led named "Scroll Lock"; sleep 0.05; done;
fi
if [ $1 = "s30" ];then
	net lookup $2
fi
if [ $1 = "s31" ];then
	echo -n "Enter password: "
	stty -echo
	read password
	stty echo
	echo ""
	echo "Your password: [$password]"
fi
if [ $1 = "s32" ];then
	7z a -p ~/passwd.7z ~/passwd
	rm ~/passwd
fi
if [ $1 = "s33" ];then
	calendar
fi
if [ $1 = "s34" ];then
	sudo apt-get install qtcreator qt5-doc qttranslations5-l10n g++ gcc cmake qtbase5-dev
fi
