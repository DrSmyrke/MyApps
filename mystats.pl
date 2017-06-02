#!/usr/bin/perl
# MyStats, a system monitor, based on DrSmyrke
#
# Any original DrSmyrke code is licensed under the BSD license
#
# All code written since the fork of DrSmyrke is licensed under the GPL
#
#
# Copyright (c) 2015 Prokofiev Y. <Smyrke2005@yandex.ru>
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
exit if ($^O eq 'MSWin32'); # под виндой мы не работаем, мы там играем
use Carp;
use Fcntl qw(:DEFAULT :flock);
use utf8;
use Encode;
use strict;
use IO::Socket;
use IO::Handle;
use IO::Select;
$SIG{INT}="sigExit";
$SIG{TERM}="sigExit";
$SIG{KILL}="sigExit";
my $version="2.0";
my $mode=0;
my $logsdir="$ENV{HOME}/.mystats/logs";
my %lookdirs=("TRASH"=>"$ENV{HOME}/.local/share/Trash/files");
my $rootPid=$$;
my %gData;
my %settings;
my $ioset=IO::Select->new;

$settings{"serverPort"}=7300;
$settings{"pidfile"}="$ENV{HOME}/.run/mystats.pid";


### Parsing arguments ###
my $i=0;
foreach my $str(@ARGV){
	if($str eq "--daemon"){$mode=1;}
	if($str eq "--stop"){stop(getMyPID($settings{"pidfile"}));sigExit();}
	if($str eq "-logs"){$logsdir=$ARGV[$i+1];}
	if($str eq "-pidfile"){$settings{"pidfile"}=$ARGV[$i+1];}
	if($str eq "-p" or $str eq "--port"){$settings{"serverPort"}=$ARGV[$i+1];}
	if($str eq "-h" or $str eq "--help"){$mode=3;}
	$i++;
}
### mode ###
print "=== MyStats v$version ===\n";
if($mode eq 0){gui();}
if($mode eq 1){daemon();}
if($mode eq 3){getHelp();exit;}



###############################
###			FUNCTIONS		###
###############################
sub getHelp{
	print "=== Help Page for MyStats by Dr.Smyrke ===\n";
	print "
-h,--help			To see the current page
--daemon			Run application as a daemon
--stop				Stopping the application
-logs <DIR>			Determination log storage directory (default: ~/.mystats)
-pidfile <FILE>			Determining PID file (default: $settings{pidfile})
-p,--port <PORT>		Determination of the port which will be run a web server (default: $settings{serverPort})
";
	print "\n=== Thank you for choosing my application ===\n";
	my ($device,$inode,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks)=stat($0);
	my $i=0;
	open FS,"<$0";
	while(my $tmp=<FS>){$i++;}
	close FS;
	print "=== The app contains $i lines of code and takes ".getSize($size)." of disk space ===\n";
}
sub stop{
	my $pid=shift @_;
	if($pid){
		print "KILLING [$pid]\n";
		kill 'TERM',$pid;
	}
}
sub getMyPID{
	my $pidf=shift @_;
	my $pid=0;
	if(!-d "$ENV{HOME}/.run"){mkdir "$ENV{HOME}/.run";}	
	if(!-f $pidf){
		print "\nCreate [$pidf] for [$$]\n";
		open fs,">$pidf";print fs "$$";close fs;
	}else{
		open fs,"<$pidf";$pid=<fs>;close fs;
	}
	return $pid;
}
sub sigExit{
	if($$ eq $rootPid){
		print "\nremove [$settings{pidfile}]";
		unlink $settings{"pidfile"};
		print "\nExit\n";
		unlink $settings{"pidfile"};
	}
	exit;
}
sub check_proc {
	my ($file) = @_;
	my $result;
    sysopen LOCK, $file, O_RDWR|O_CREAT or croak "Don`t open file $file: $!";
    if ( flock LOCK, LOCK_EX|LOCK_NB  ) {
        truncate LOCK, 0 or croak "Don`t lock file $file: $!";
        my $old_fh = select LOCK;
        $| = 1;
        select $old_fh;
        print LOCK $$;
    }
    else {
        $result = <LOCK>;
        if (defined $result) {
            chomp $result;
        }else{
            carp "PID not found in file $file";
            $result = '0 but true';
        }
    }
    return $result;
}
sub gui{
#	$vis=0;
#	$vismode=0;
#	$controlMode=0;
	my $winwidth=270;							# Starting window width
	my $winheight=450;							# Starting window height
	# background color
	$settings{"bgColorR"}=parsRGB(0);
	$settings{"bgColorG"}=parsRGB(0);
	$settings{"bgColorB"}=parsRGB(0);
	$settings{"bgColorA"}=0.8;
	# clock param
	$settings{"clockColorR"}=parsRGB(229);
	$settings{"clockColorG"}=parsRGB(191);
	$settings{"clockColorB"}=parsRGB(14);
	$settings{"clockColorA"}=1;
	$settings{"clockSize"}=30;
	$settings{"clockWidth"}="bold";
	$settings{"clockStyle"}="normal";
	$settings{"clockFont"}="Sans";
	$settings{"yi"}=15;								# Расстояние между строчками
	$settings{"cx"}=5;								# Отступ справа по X
	# Default font style
	$settings{"defaultTextFont"}="Sans";
	$settings{"defaultTextStyle"}="normal";
	$settings{"defaultTextWidth"}="normal";
	$settings{"defaultTextSize"}=12;
	$settings{"defaultText"}=($settings{"defaultTextFont"},$settings{"defaultTextStyle"},$settings{"defaultTextWidth"},$settings{"defaultTextSize"});
	# Default font color
	$settings{"defTextValR"}=parsRGB(138);
	$settings{"defTextValG"}=parsRGB(186);
	$settings{"defTextValB"}=parsRGB(56);
	$settings{"defTextValA"}=1;

	$settings{"defTextParamR"}=parsRGB(255);
	$settings{"defTextParamG"}=parsRGB(175);
	$settings{"defTextParamB"}=parsRGB(0);
	$settings{"defTextParamA"}=1;

	# Progressbar style
	$settings{"progressBar3d"}=1;
	$settings{"progressBarBlockWidth"}=3;
	$settings{"progressBarBlockHeight"}=12;
	$settings{"progressBarBlockIntery"}=1;
	$settings{"progressBarR"}=parsRGB(100);
	$settings{"progressBarG"}=parsRGB(100);
	$settings{"progressBarB"}=parsRGB(100);
	$settings{"progressBarA"}=0.7;


	# CPU calculate
	$gData{"prev_idle"}=0;
	$gData{"prev_total"}=0;

#	if(getMyPID($pidfile)){print "NO EXECUTE DUBLICATE APPS\n";exit;}
	my $pid = check_proc($settings{"pidfile"});
	if($pid){die "Proccess #$pid is running, NO EXECUTE DUBLICATE APPS!\n";exit;}
	print "STARTING ...\n";
	eval "use Gtk2 '-init';";
	eval "use Cairo;";
	my $win=Gtk2::Window->new('popup');
	$win->set_decorated(0);
	$win->set_resizable(1);
	$win->set_keep_above('TRUE');
	$win->set_skip_taskbar_hint('TRUE');
	$win->set_skip_pager_hint('TRUE');
	$win->stick();
	$win->set_app_paintable('TRUE');
	$win->set_default_size($winwidth,$winheight);
	screen_changed($win);
	$win->show_all();
	$win->signal_connect(delete_event=>sub{Gtk2->main_quit;});
	$win->signal_connect(destroy=>sub{Gtk2->main_quit;});
	$win->signal_connect(leave_notify_event=>sub{
		$gData{"uiShow"}=0;
		$gData{"mouseEnter"}=0;
		$gData{"mouseEnterCounter"}=0;
		$gData{"hoverDisk"}="";
		$gData{"ejectDist"}="";
		$gData{"mouseX"}=0;
		$gData{"mouseY"}=0;
	});
	$win->signal_connect(enter_notify_event=>sub{
		$gData{"uiShow"}=1;
		$gData{"mouseEnter"}=1;
	});
	$win->signal_connect("screen_changed",\&screen_changed,$win);
	$win->signal_connect("button_release_event",\&win_clicked);
	$win->signal_connect("motion_notify_event",\&motion_notify_event);
	$win->add_events(["GDK_BUTTON_RELEASE_MASK","GDK_POINTER_MOTION_MASK"]);

	my $timer = Glib::Timeout->add(250,\&updUI,$win); 
	my $timer2 = Glib::Timeout->add(1000,\&updData,undef);
	my $timer3 = Glib::Timeout->add(60000,\&saveLogs,undef);


	$gData{"win"}=$win;
	$gData{"uiShow"}=0;
	$settings{"winAutohide"}=1;
	
	#setUIpos($win);
	updUI($win);
	drawUI($win);

	Gtk2->main;
}
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
sub getSize{my $st;my ($sz)=@_;if($sz<1024){$st="$sz  bytes";}else{if($sz>1024 and $sz<1024000){$sz=substr($sz/1024,0,5);$st="$sz Kb";}else{if($sz>1024000 and  $sz<1024000000){$sz=substr($sz/1048576,0,5);$st="$sz Mb";}else{if($sz>1024000000){$sz=substr($sz/1048576000,0,5);$st="$sz Gb";}}}}return $st;}
sub getBar{
	my($cr,$x,$y,$val,$blockCount)=@_;
	my $pbarWidth=($settings{"progressBarBlockWidth"}*$blockCount)+($settings{"progressBarBlockIntery"}*($blockCount-1));
	my $prz=($pbarWidth/100.0)*$val;
	$cr->set_source_rgba($settings{"progressBarR"},$settings{"progressBarG"},$settings{"progressBarB"},$settings{"progressBarA"});
	my $rx=$x;
	for(my $i=0;$i<$blockCount;$i++){
		$cr->rectangle($rx,$y,$settings{"progressBarBlockWidth"},$settings{"progressBarBlockHeight"});
		$cr->fill();
		$rx=$rx+$settings{"progressBarBlockIntery"}+$settings{"progressBarBlockWidth"};
	}
	my $pattern=Cairo::LinearGradient->create($x,$y+($settings{"progressBarBlockHeight"}/2),$x+$pbarWidth,$y+($settings{"progressBarBlockHeight"}/2));
	$pattern->add_color_stop_rgba(0,0,0.7,0,$settings{"progressBarA"});
	$pattern->add_color_stop_rgba(1,0.7,0,0,$settings{"progressBarA"});
	$cr->set_source($pattern);
	$rx=$x;
	for(my $i=0;$i<$blockCount;$i++){
		if(($rx-$x)<$prz){
			$cr->rectangle($rx,$y,$settings{"progressBarBlockWidth"},$settings{"progressBarBlockHeight"});
			$cr->fill();
		}

		$rx=$rx+$settings{"progressBarBlockIntery"}+$settings{"progressBarBlockWidth"};
	}

	if($settings{"progressBar3d"}){
		#draw 3d
		$cr->set_source_rgba(0,0,0,0.3);
		$cr->set_line_width(0.3);
		$rx=$x;
		for(my $i=0;$i<$blockCount;$i++){
			$cr->move_to($rx,$y+($settings{"progressBarBlockHeight"}/2));
			$cr->line_to($rx+$settings{"progressBarBlockWidth"},$y+($settings{"progressBarBlockHeight"}/2));
			$cr->stroke();
			$cr->rectangle($rx,$y+($settings{"progressBarBlockHeight"}/2),$settings{"progressBarBlockWidth"},$settings{"progressBarBlockHeight"}/2);
			$cr->fill();
			$rx=$rx+$settings{"progressBarBlockIntery"}+$settings{"progressBarBlockWidth"};
		}
	}

	my $str="$val%";
	$str=~s/,/./g;
	$cr->move_to($x+($pbarWidth/2)-(length($str)*4),$y+10);
	$cr->set_source_rgba($settings{"clockColorR"},$settings{"clockColorG"},$settings{"clockColorB"},1);
	$cr->show_text($str);
}
sub printText{
	my ($cr,$font,$style,$type,$size,$xpos,$ypos,$colr,$colg,$colb,$cola,$text)=@_;
	$cr->select_font_face($font,$style,$type);
	$cr->set_font_size($size);
	$cr->move_to($xpos,$ypos);
	$cr->set_source_rgba($colr,$colg,$colb,$cola);
	$cr->show_text($text);
}
sub printParam{
	my ($cr,$xpos,$ypos,$text)=@_;
	$cr->select_font_face($settings{"defaultTextFont"},$settings{"defaultTextStyle"},$settings{"defaultTextWidth"});
	$cr->set_font_size($settings{"defaultTextSize"});
	$cr->move_to($xpos,$ypos);
	$cr->set_source_rgba($settings{"defTextParamR"},$settings{"defTextParamG"},$settings{"defTextParamB"},$settings{"defTextParamA"});
	$cr->show_text($text);
}
sub printValue{
	my ($cr,$xpos,$ypos,$text)=@_;
	$cr->select_font_face($settings{"defaultTextFont"},$settings{"defaultTextStyle"},$settings{"defaultTextWidth"});
	$cr->set_font_size($settings{"defaultTextSize"});
	$cr->move_to($xpos,$ypos);
	$cr->set_source_rgba($settings{"defTextValR"},$settings{"defTextValG"},$settings{"defTextValB"},$settings{"defTextValA"});
	$cr->show_text($text);
}
sub printDate{
	my ($cr,$xpos,$ypos,$text)=@_;
	$cr->select_font_face($settings{"defaultTextFont"},$settings{"defaultTextStyle"},$settings{"defaultTextWidth"});
	$cr->set_font_size($settings{"defaultTextSize"});
	$cr->move_to($xpos,$ypos);
	$cr->set_source_rgba($settings{"defTextValR"},$settings{"defTextValG"}+0.5,$settings{"defTextValB"},$settings{"defTextValA"});
	$cr->show_text($text);
}
sub parsRGB{
	my ($val)=@_;
	if($val > 0){$val=($val/2.55)/100;}else{$val=0;}
	$val=~s/,/./g;
	return $val;
}
sub screen_changed{
	my ($widget)=@_;
	my $screen=$widget->get_screen();
	my $tmp2=$screen->get_rgba_colormap();
	print "SCREEN CHANGED [";
	if(!$tmp2){
		print "Your screen does not support alpha channel";
		$gData{"supportAlpha"}=0;
		$tmp2=$screen->get_rgb_colormap();
	}else{
		$gData{"supportAlpha"}=1;
		print "Your screen supports alpha channel";
	}
	$widget->set_colormap($tmp2);
	print "]\n";
	$gData{"screenWidth"}=$screen->get_width;
	$gData{"screenHeight"}=$screen->get_height;
}

sub win_clicked{
	my ($widget,$event) = @_;
	my $button_code = $event->button();
	my $action=0;
	#print "RELEASE: $button_code [$gData{hoverDisk}]\n";
	# open disk
	if($gData{"hoverDisk"} ne "" and $gData{"ejectDisk"} eq "" and $button_code eq 1){
		if(fork==0){exec "xdg-open $gData{hoverDisk}";kill 'TERM',$$;}
		$action=1;
	}
	# eject disk
	if($gData{"hoverDisk"} ne "" and $gData{"ejectDisk"} ne "" and $button_code eq 1){
		my $part=$gData{"data"}{"disks"}{$gData{"ejectDisk"}}{"dev"};
		my $dev=substr($part,0,-1);
		my $cmd="udisks --unmount $part || udisksctl unmount -b $part;udisks --detach $dev || udisksctl power-off -b $dev;notify-send -i gnome-terminal 'Операции с устройствами' 'Устройство успешно извлечено!'";
		if(fork==0){exec "$cmd";kill 'TERM',$$;}
		$gData{"hoverDisk"}="";
		$gData{"ejectDist"}="";
		$action=1;
	}

	if(!$action and $button_code eq 1){$settings{"winAutohide"}=($settings{"winAutohide"} eq 1)?0:1;}
#	if(!$action and $button_code eq 2){
#		$settings{"server"}=($settings{"server"} eq 1)?0:1;
#		if($webserver eq -1){$settings{"server"}=0;}
#		if($settings{"server"}){
#			$settings{"saveLogs"}=1;
#			#$thread=threads->create(\&startWebServer,$param);
#			$settings{"serverThread"}=threads->create(\&startWebServer,undef);
#		}else{
#			if(exists($settings{"serverThread"}) and $webserver ne -1){
#				print "WEB SERVER SHUTDOWN ... ";
#				$settings{"serverThread"}->kill('STOP')->detach();
#			}
#			$settings{"server"}=0;
#		}
#	}
	if(!$action and $button_code eq 3){$settings{"saveLogs"}=($settings{"saveLogs"} eq 1)?0:1;}
	return 1;
}
sub updUI{
	my ($widget) = @_;
	setUIpos($widget);
	if($gData{"winShow"}){drawUI($widget);}

	
	if($gData{"winHeight"}!=$gData{"contHeight"} and $gData{"contHeight"}>0){
		$gData{"win"}->resize($gData{"winWidth"},$gData{"contHeight"});
	}
	return 1;
}
sub motion_notify_event{
	my ($widget,$event) = @_;
	$gData{"mouseX"}=$event->x;
	$gData{"mouseY"}=$event->y;
}
sub setUIpos{
	my ($win) = @_;
	my($w,$h)=$win->get_size;
	my($x,$y)=$win->get_position;
	$gData{"winWidth"}=$w;
	$gData{"winHeight"}=$h;
	my($nx,$ny)=0;
	if($gData{"uiShow"}==1 or !$settings{"winAutohide"}){
		$gData{"winShow"}=1;
		$nx=$gData{"screenWidth"}-$w;
	}else{
		$gData{"winShow"}=0;
		$nx=$gData{"screenWidth"}-4;
	}
	if($x ne $nx and $y ne $ny){$win->move($nx,$ny);}
}
sub nextLine{$gData{"contHeight"}+=$settings{"yi"};}
sub drawUI{
	my ($widget)=@_;
	my $cr=Gtk2::Gdk::Cairo::Context->create($widget->window());
	$cr->set_operator('source');
	if($gData{"supportAlpha"} eq 1){
		$cr->set_source_rgba($settings{"bgColorR"},$settings{"bgColorG"},$settings{"bgColorB"},$settings{"bgColorA"});
	}else{
		$cr->set_source_rgb($settings{"bgColorR"},$settings{"bgColorG"},$settings{"bgColorB"});
	}
	#$cr->rectangle(0,0,$gData{"winWidth"},$gData{"contHeight"}+$gData{"yi"});
	#$cr->fill();
	#$cr->stroke();
	$cr->paint();
	$gData{"contHeight"}=0;
	$cr=Gtk2::Gdk::Cairo::Context->create($widget->window());
	
	if($gData{"mouseEnter"}){$gData{"mouseEnterCounter"}++;}
	
	if($gData{"mouseEnter"} and $gData{"mouseEnterCounter"}>30){
		nextLine();
		printText($cr,"Sans","normal","bold",12,$settings{"cx"},$gData{"contHeight"}
			,parsRGB(255),parsRGB(168),parsRGB(0),1,"MyStats by Dr.Smyrke");
		nextLine();
		printText($cr,"Sans","normal","normal",12,$settings{"cx"},$gData{"contHeight"}
			,parsRGB(255),parsRGB(168),parsRGB(0),1,'smyrke2005@yandex.ru');
		nextLine();
		printText($cr,"Sans","normal","bold",12,$settings{"cx"},$gData{"contHeight"}
			,parsRGB(255),parsRGB(168),parsRGB(0),1,"--- KEY MAPS ------------");
		nextLine();
		printText($cr,"Sans","normal","normal",10,$settings{"cx"},$gData{"contHeight"}
			,parsRGB(130),parsRGB(130),parsRGB(130),1,"MouseLeft");
		printText($cr,"Sans","normal","normal",10,$settings{"cx"}+160,$gData{"contHeight"}
			,parsRGB(130),parsRGB(130),parsRGB(130),1,"Fixed Window");
		nextLine();
		printText($cr,"Sans","normal","normal",10,$settings{"cx"},$gData{"contHeight"}
			,parsRGB(130),parsRGB(130),parsRGB(130),1,"MouseRight");
		printText($cr,"Sans","normal","normal",10,$settings{"cx"}+160,$gData{"contHeight"}
			,parsRGB(130),parsRGB(130),parsRGB(130),1,"Enable logging");
		nextLine();
	}
	if(!$settings{"winAutohide"}){
		printText($cr,$settings{"clockFont"},"normal","bold",9,170,16,255,0,0,1,"FIXED");
		$cr->move_to(168,5);
		$cr->rel_line_to(35,0);
		$cr->rel_line_to(0,15);
		$cr->rel_line_to(-35,0);
		$cr->rel_line_to(0,-16);
		$cr->stroke();
	}
	#log
	if($settings{"saveLogs"}){
		printText($cr,$settings{"clockFont"},"normal","bold",9,210,16,255,0,0,1,"LOGING");
		$cr->move_to(208,5);
		$cr->rel_line_to(44,0);
		$cr->rel_line_to(0,15);
		$cr->rel_line_to(-44,0);
		$cr->rel_line_to(0,-16);
		$cr->stroke();
	}
	# clock
	$gData{"contHeight"}+=25;
	printText($cr,$settings{"clockFont"},$settings{"clockStyle"},$settings{"clockWidth"}
			,$settings{"clockSize"},$settings{"cx"},$gData{"contHeight"}
			,$settings{"clockColorR"},$settings{"clockColorG"},$settings{"clockColorB"}
			,$settings{"clockColorA"},$gData{"data"}{"time"});
	# uptime
	nextLine();
	printParam($cr,$settings{"cx"},$gData{"contHeight"},"Uptime:");
	printValue($cr,$settings{"cx"}+60,$gData{"contHeight"},$gData{"data"}{"uptime"});
	printDate($cr,$settings{"cx"}+150,$gData{"contHeight"},$gData{"data"}{"date"});
	# cpu
	nextLine();
	printParam($cr,$settings{"cx"},$gData{"contHeight"},"CPU:");
	getBar($cr,$settings{"cx"}+60,$gData{"contHeight"}-10,$gData{"data"}{"cpu"},50);
	# MEM
	nextLine();
	printParam($cr,$settings{"cx"},$gData{"contHeight"},"RAM:");
	getBar($cr,$settings{"cx"}+60,$gData{"contHeight"}-10,$gData{"data"}{"ram"},15);
	printValue($cr,$settings{"cx"}+125,$gData{"contHeight"},getSize($gData{"data"}{"mem_used"})." / ".getSize($gData{"data"}{"mem_total"}));
	# SWAP
	nextLine();
	printParam($cr,$settings{"cx"},$gData{"contHeight"},"SWAP:");
	getBar($cr,$settings{"cx"}+60,$gData{"contHeight"}-10,$gData{"data"}{"swap"},15);
	printValue($cr,$settings{"cx"}+125,$gData{"contHeight"},getSize($gData{"data"}{"swap_used"})." / ".getSize($gData{"data"}{"swap_total"}));
	# APPS
	nextLine();
	printParam($cr,$settings{"cx"},$gData{"contHeight"},"Apps:");
	printValue($cr,$settings{"cx"}+60,$gData{"contHeight"},$gData{"data"}{"apps"});
	# IFACES
	if(exists($gData{"data"}{"ifaces"})){
		foreach my $iface(keys $gData{"data"}{"ifaces"}){
			nextLine();
			$cr->set_source_rgba(0.9,0.9,0.9,0.8);
			$cr->move_to($settings{"cx"},$gData{"contHeight"});
			$cr->line_to($settings{"cx"}+10,$gData{"contHeight"});
			$cr->move_to($settings{"cx"}+70,$gData{"contHeight"});
			$cr->line_to($gData{"winWidth"}-$settings{"cx"},$gData{"contHeight"});
			$cr->stroke();
			printValue($cr,$settings{"cx"}+15,$gData{"contHeight"}+3,$iface);

			nextLine();
			printParam($cr,$settings{"cx"},$gData{"contHeight"},"Down:");
			printValue($cr,$settings{"cx"}+60,$gData{"contHeight"},getSize($gData{"data"}{"ifaces"}{$iface}{"speedDown"})." / s");
			printValue($cr,$settings{"cx"}+180,$gData{"contHeight"},"+ ".getSize($gData{"data"}{"ifaces"}{$iface}{"download"}));
	
			nextLine();
			printParam($cr,$settings{"cx"},$gData{"contHeight"},"Up:");
			printValue($cr,$settings{"cx"}+60,$gData{"contHeight"},getSize($gData{"data"}{"ifaces"}{$iface}{"speedUp"})." / s");
			printValue($cr,$settings{"cx"}+180,$gData{"contHeight"},"+ ".getSize($gData{"data"}{"ifaces"}{$iface}{"upload"}));

			nextLine();
			printParam($cr,$settings{"cx"},$gData{"contHeight"},"IP:");
			printValue($cr,$settings{"cx"}+60,$gData{"contHeight"},$gData{"data"}{"ifaces"}{$iface}{"ip"});
			printValue($cr,$settings{"cx"}+180,$gData{"contHeight"},"= ".getSize($gData{"data"}{"ifaces"}{$iface}{"total"}));
		}
	}
	nextLine();
	# DISKS
	if(exists($gData{"data"}{"disks"})){
		foreach my $mount(sort keys $gData{"data"}{"disks"}){
			nextLine();
			#draw Eject
			my $drawEject=0;
			if($mount ne "/"){
				if($gData{"mouseY"}>=$gData{"contHeight"}-10 and $gData{"mouseY"}<$gData{"contHeight"}+$settings{"yi"}){
					$cr->set_source_rgba(0.9,0.9,0.9,0.1);
					if($gData{"hoverDisk"} eq $mount){$gData{"hoverDiskCounter"}++;}else{$gData{"hoverDiskCounter"}=0;}
					if($gData{"hoverDiskCounter"}>1){
						$drawEject=1;
						if($gData{"mouseX"}>=$gData{"winWidth"}-37 and $gData{"mouseX"}<$gData{"winWidth"}-15){
							$cr->set_source_rgba(1,0,0,0.1);
						}
					}
					$cr->rectangle(3,$gData{"contHeight"}-12,$gData{"winWidth"}-6,$settings{"yi"}*2);
					$cr->fill();
					$gData{"hoverDisk"}="$mount";
				}
			}

			my $nazv=$gData{"data"}{"disks"}{$mount}{"name"};
			if(length($nazv)>4){$nazv=substr($nazv,0,4);$nazv="$nazv...";}
			printParam($cr,$settings{"cx"},$gData{"contHeight"},"[$nazv]");

			if($drawEject){
				getBar($cr,$settings{"cx"}+60,$gData{"contHeight"}-10,$gData{"data"}{"disks"}{$mount}{"usedPrz"},37);
				# draw eject icon
				$cr->set_source_rgba(0.9,0.9,0.9,0.2);
				if($gData{"mouseX"}>=$gData{"winWidth"}-37 and $gData{"mouseX"}<$gData{"winWidth"}-15){
					$cr->set_source_rgba($settings{"clockColorR"},$settings{"clockColorG"},$settings{"clockColorB"},$settings{"clockColorA"});
					$cr->rectangle($gData{"winWidth"}-38,$gData{"contHeight"}-7,30,20);
					$cr->stroke();
					$gData{"ejectDisk"}=$gData{"hoverDisk"};
				}else{
					$gData{"ejectDisk"}="";
				}
				$cr->move_to($gData{"winWidth"}-35,$gData{"contHeight"}+3);
				$cr->rel_line_to(12,-7);
				$cr->rel_line_to(12,7);
				$cr->rel_line_to(-24,0);
				$cr->fill();
				$cr->rectangle($gData{"winWidth"}-35,$gData{"contHeight"}+5,24,3);
				$cr->fill();
			}else{
				getBar($cr,$settings{"cx"}+60,$gData{"contHeight"}-10,$gData{"data"}{"disks"}{$mount}{"usedPrz"},50);
			}
			nextLine();
			printParam($cr,$settings{"cx"},$gData{"contHeight"},"Free:");
			printValue($cr,$settings{"cx"}+60,$gData{"contHeight"},getSize($gData{"data"}{"disks"}{$mount}{"avail"})." / ".getSize($gData{"data"}{"disks"}{$mount}{"size"}));
		}
	}
	nextLine();
	# DIRS
	if(exists($gData{"data"}{"dirs"})){
		foreach my $dir(sort keys $gData{"data"}{"dirs"}){
			nextLine();
			printParam($cr,$settings{"cx"},$gData{"contHeight"},$dir);
			printValue($cr,$settings{"cx"}+180,$gData{"contHeight"},getSize($gData{"data"}{"dirs"}{$dir}{"size"}));
		}
	}
	nextLine();
}
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
sub updData{
	($gData{"data"}{"time"},$gData{"data"}{"date"},$gData{"data"}{"logfile"})=date();
	$gData{"data"}{"uptime"}=uptime();
	$gData{"data"}{"cpu"}=cpu();
	($gData{"data"}{"ram"},$gData{"data"}{"mem_total"},$gData{"data"}{"mem_used"},$gData{"data"}{"swap"},$gData{"data"}{"swap_total"},$gData{"data"}{"swap_used"})=mem();
	$gData{"data"}{"apps"}=proc();
	ifaces();
	disks();
	dirs();
	return 1;
}
sub date{
	my ($sek,$min,$hour,$day,$mon,$year,$wday)=localtime;
	if($wday eq 1){$wday="Пн";}
	if($wday eq 2){$wday="Вт";}
	if($wday eq 3){$wday="Ср";}
	if($wday eq 4){$wday="Чт";}
	if($wday eq 5){$wday="Пт";}
	if($wday eq 6){$wday="Сб";}
	if($wday eq 0){$wday="Вс";}
	$mon++;
	$year+=1900;
	if($day<10){$day="0$day";}
	if($mon<10){$mon="0$mon";}
	if($hour<10){$hour="0$hour";}
	if($min<10){$min="0$min";}
	if($sek<10){$sek="0$sek";}
	return "$hour:$min:$sek","$wday $day.$mon.$year","$year-$mon-$day";
}
sub uptime{
	open FS,"</proc/uptime";my ($uptime)=split(" ",<FS>);close FS;
	my $h=int $uptime/60/60;
	my $m=int (($uptime-($h*60*60))/60);
	my $s=int ($uptime-($h*60*60)-($m*60));
	if($h<10){$h="0$h";}
	if($m<10){$m="0$m";}
	if($s<10){$s="0$s";}
	return "$h:$m:$s";
}
sub cpu{
	open FS,"</proc/stat";
	my $cpu=0;
	while(my $str=<FS>){
		chomp $str;
		my @tmp=split(" ",$str);
		shift @tmp;
		my $idle=$tmp[3];
		my $total=0;
		foreach my $tmp2(@tmp){$total+=$tmp2;}
		my $diff_idle=$idle-$gData{"prev_idle"};
		my $diff_total=$total-$gData{"prev_total"};
		$cpu=int(1000*($diff_total-$diff_idle)/$diff_total+5)/10;
		$gData{"prev_total"}=$total;
		$gData{"prev_idle"}=$idle;
		last;
	}
	close FS;
	return $cpu;
}
sub mem{
	my $mem="";
	my @tmp=`free -b`;
	my (undef,$mem_t,$mem_u)=split(" ",$tmp[1]);
	#($tmp2,$tmp2,$mem_u)=split(" ",$tmp[2]);
	#$mem=substr(($mem_u*100/$mem_t),0,4);
	$mem=$mem_u/($mem_t/100);
 	my (undef,$swap_t,$swap_u)=split(" ",$tmp[2]);
	my $swap;
	if($swap_t eq 0 and $swap_u eq 0){
		$swap=0;
	}else{
		$swap=$swap_u/($swap_t/100);
	}
	return substr($mem,0,5),$mem_t,$mem_u,substr($swap,0,5),$swap_t,$swap_u;
}
sub proc{
	my $i=0;
	foreach my $str(glob("/proc/*")){
		my @tmp=split("/",$str);
		if(pop(@tmp)>0){$i++;}
	}
	return $i;
}
sub ifaces{
	open FS,"</proc/net/dev";
	while(my $str=<FS>){
		if($str=~/:/){
			my ($iface,$traffd,undef,undef,undef,undef,undef,undef,undef,$traffu)=split(" ",$str);
			$iface=~s/://g;
			if($iface eq "lo"){next;}
			my $cmd=`ifconfig $iface | grep "inet addr"`;chomp $cmd;
			if($cmd=~/Устройство не обнаружено/){next;}
			my $ip;
			if($cmd eq ""){
				$ip="No Address";
			}else{
				my ($tmp2,$tmp)=split("inet addr:",$cmd);
				($ip)=split(" ",$tmp);
			}
			my $tdown=$traffd-$gData{"data"}{"ifaces"}{"$iface"}{"download"};
			my $tupl=$traffu-$gData{"data"}{"ifaces"}{"$iface"}{"upload"};
			my $ttot=$traffd+$traffu;
			$gData{"data"}{"ifaces"}{"$iface"}{"ip"}=$ip;
			$gData{"data"}{"ifaces"}{"$iface"}{"total"}=$ttot;
			$gData{"data"}{"ifaces"}{"$iface"}{"speedDown"}=$tdown;
			$gData{"data"}{"ifaces"}{"$iface"}{"speedUp"}=$tupl;
			$gData{"data"}{"ifaces"}{"$iface"}{"download"}=$traffd;
			$gData{"data"}{"ifaces"}{"$iface"}{"upload"}=$traffu;
		}
	}
	close FS;
}
sub disks{
	delete $gData{"data"}{"disks"};
	my @cmd=`df -B 1`;shift @cmd;chomp @cmd;
	foreach my $str(@cmd){
		my ($fs,$tsize,$usize,$dsize,$dprz,$mount)=split(" ",$str);
		if($fs eq "none" or $fs eq "udev" or $fs eq "tmpfs" or $fs eq "cgmfs"){next;}
		my $dprz=$usize/($tsize/100);
		$dprz=substr($dprz,0,5);
		$gData{"data"}{"disks"}{$mount}{"usedPrz"}=$dprz;
		if($mount ne "/"){
			my @tmp2=split("/",$mount);
			$gData{"data"}{"disks"}{$mount}{"name"}=$tmp2[@tmp2-1];
		}else{$gData{"data"}{"disks"}{$mount}{"name"}=$mount;}
		$gData{"data"}{"disks"}{$mount}{"dev"}=$fs;
		$gData{"data"}{"disks"}{$mount}{"size"}=$tsize;
		$gData{"data"}{"disks"}{$mount}{"used"}=$usize;
		$gData{"data"}{"disks"}{$mount}{"avail"}=$tsize-$usize;
	}
}
sub dirs{
	delete $gData{"data"}{"dirs"};
	foreach my $key(keys %lookdirs){
		my $dir=$lookdirs{$key};
		my @size=`if [ -e $dir ]; then du -abc $dir | grep "итого" || du -abc $dir | grep "total"; fi`;chomp @size;
		my $tmp=@size;my ($sz)=split(" ",$size[$tmp-1]);
		$gData{"data"}{"dirs"}{$key}{"dir"}=$dir;
		$gData{"data"}{"dirs"}{$key}{"size"}=$sz;
	}
}
sub saveLogs{
	if(!$settings{"saveLogs"}){return 1;}
	if(!-d $logsdir){mkdir $logsdir;}
	if(!-d $logsdir){print "[Can not find the log directory]\n";return 1;}
	open LFS,">>$logsdir/$gData{data}{logfile}.log";
	print LFS "time:$gData{data}{time}	cpu:$gData{data}{cpu}	ram:$gData{data}{ram}:$gData{data}{mem_total}:$gData{data}{mem_used}	swap:$gData{data}{swap}:$gData{data}{swap_total}:$gData{data}{swap_used}	apps:$gData{data}{apps}";
	if(exists($gData{"data"}{"ifaces"})){
		print LFS " ifaces:";
		my @out;
		foreach my $iface(keys $gData{"data"}{"ifaces"}){
			push(@out,"$iface+++$gData{data}{ifaces}{$iface}{download}+++$gData{data}{ifaces}{$iface}{upload}+++$gData{data}{ifaces}{$iface}{speedDown}+++$gData{data}{ifaces}{$iface}{speedUp}+++$gData{data}{ifaces}{$iface}{total}");
		}
		print LFS join("===",@out);
	}
	if(exists($gData{"data"}{"disks"})){
		print LFS " disks:";
		my @out;
		foreach my $mount(keys $gData{"data"}{"disks"}){
			push(@out,"$mount+++$gData{data}{disks}{$mount}{name}+++$gData{data}{disks}{$mount}{dev}+++$gData{data}{disks}{$mount}{size}+++$gData{data}{disks}{$mount}{used}+++$gData{data}{disks}{$mount}{avail}");
		}
		print LFS join("===",@out);
	}
	if(exists($gData{"data"}{"disks"})){
		print LFS " dirs:";
		my @out;
		foreach my $dir(keys $gData{"data"}{"dirs"}){
			push(@out,"$dir+++$gData{data}{dirs}{$dir}{dir}+++$gData{data}{dirs}{$dir}{size}");
		}
		print LFS join("===",@out);
	}
	print LFS "\n";
	close LFS;
	return 1;
}
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
########################################################################################################
sub daemon{
	print "DAEMON ... ";
	my $pid = check_proc($settings{"pidfile"});
	if($pid){die "Proccess #$pid is running, NO EXECUTE DUBLICATE APPS!\n";exit;}
	print " [ STARTING ]\n";
	my ($updCounter,$logCounter)=0;
	$settings{"saveLogs"}=1;
	while(1){
		if($updCounter eq 2){updData();$updCounter=0;}else{$updCounter++;}
		if($logCounter eq 120){saveLogs();$logCounter=0;}else{$logCounter++;}
		select(undef,undef,undef,0.5);
	}
}
