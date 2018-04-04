#!/bin/bash
 #положить в /etc/profile.d
PROCCOUNT=`ps -Afl | wc -l`
PROCCOUNT=`expr $PROCCOUNT - 5`
 
CPUCOUNT=`lscpu | grep "CPU(s):" | head -1 | awk '{print $2}'`
CPUMODEL=`cat /proc/cpuinfo | grep "model name" | head -1 | awk '{print $4" "$5" "$6" "$7" "$9}'`
MEMKB=`cat /proc/meminfo | grep MemTotal | awk {'print $2'}`
MEMMB=`echo "$MEMKB / 1024" | bc `
IPS=`/sbin/ifconfig -a | grep "inet " | awk '{print $2}' | awk -F':' '{print $2}' | grep -v 127.0.0.1`
 
for i in $IPS; do ip="$i $ip"; done
echo -e "
\033[0;35m+++++++++++++++++: \033[0;37mSystem Data\033[0;35m :+++++++++++++++++++
  \033[0;37mHostname \033[0;35m= \033[1;32m`hostname`
   \033[0;37mAddress \033[0;35m= \033[1;32m$ip
    \033[0;37mKernel \033[0;35m= \033[1;32m`uname -r`
    \033[0;37mUptime \033[0;35m=\033[1;32m`uptime | sed 's/.*up ([^,]*), .*/1/'`
       \033[0;37mCPU \033[0;35m= \033[1;32m$CPUCOUNT x $CPUMODEL
    \033[0;37mMemory \033[0;35m= \033[1;32m$MEMMB Mb
\033[m
"
