#!/usr/bin/perl
# myexec, application, based on DrSmyrke
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
use utf8;
use Encode;
use strict;
use Time::Local;
use Time::Local 'timelocal_nocheck';
use Gtk2 '-init';
my $field;
my $oldText;
my @vals;

my $win=Gtk2::Window->new('toplevel');
$win->set_title("MyExec v0.2");
$win->set_position("GTK_WIN_POS_CENTER_ALWAYS");
$win->set_resizable(0);
$win->set_keep_above('TRUE');
$win->set_skip_taskbar_hint('TRUE');
$win->set_skip_pager_hint('TRUE');
$win->stick();


my $hbox=Gtk2::HBox->new(0,0);
$field=Gtk2::Entry->new();
	$field->set_size_request(300,30);
	$field->signal_connect("activate"=>sub{
		start($field->get_text());
	});
my $completion = Gtk2::EntryCompletion->new;
	$completion->set_minimum_key_length(2);
	$completion->set_text_column (0);
	$field->set_completion($completion);
my $execB=Gtk2::Button->new("Выполнить");
	$execB->set_image(Gtk2::Image->new_from_stock("GTK_STOCK_EXECUTE","GTK_ICON_SIZE_MENU"));
	$execB->set_can_focus(0);
	$execB->signal_connect("clicked"=>sub{
		start($field->get_text());
	});
$hbox->pack_start($field,1,1,0);
$hbox->pack_start($execB,0,0,0);



#	my $timer = Glib::Timeout->add(250,\&updUI,$win); 
	my $timer = Glib::Timeout->add(100,\&upd,undef);
#	my $timer3 = Glib::Timeout->add(60000,\&saveLogs,undef);
$win->add($hbox);
$win->show_all;

$win->signal_connect(delete_event=>sub{Gtk2->main_quit;});
$win->signal_connect(destroy=>sub{Gtk2->main_quit;});
$field->signal_connect(key_release_event=>sub{
	my ($win,$event)=@_;
	my $key=$event->keyval;
	my ($w,$h)=$field->get_size_request();

	if(length($field->get_text())>50){
		if($w ne 500){$field->set_size_request(500,30);}
	}else{
		if($w ne 300){$field->set_size_request(300,30);}
	}

	if($key eq 65289){return 1;}#TAB
	return 0;
});
$win->signal_connect(key_release_event=>sub{
	my ($win,$event)=@_;
	my $key=$event->keyval;
	if($key eq 65307){Gtk2->main_quit;} #ESC

	if($key eq 65289){
		if($vals[0] ne ""){
			$field->set_text($vals[0]);
			#$field->select_region(0,length($field->get_text()));
			$field->grab_focus();
			$field->set_position(length($field->get_text()));
		}
		return 1;
	}	#TAB
	return 0;
});

Gtk2->main;


sub start{
	my ($text)=@_;
	if($text eq ""){return;}
	my $cmd="";
	if(substr($text,0,1) eq "/" or substr($text,0,2) eq "#/"){
		$cmd="xdg-open $text";
		if(substr($text,0,2) eq "#/"){
			$text=substr($text,1);
			$cmd="gksu xdg-open $text || ksudo xdg-open $text";
		}
	}
	if(substr($text,0,7) eq "http://" or substr($text,0,8) eq "https://"){$cmd="xdg-open $text";}
	if(substr($text,0,1) eq "^"){$cmd=substr($cmd,1);$cmd="xterm -e '$cmd'";}
	if(substr($text,0,1) eq "!"){$cmd=substr($cmd,1);$cmd="gksu '$cmd' || ksudo '$cmd'";}
	if($cmd){if(fork==0){exec "$cmd || notify-send 'Application not execution!' '$cmd'";kill 'TERM',$$;}}
	Gtk2->main_quit;
}
sub upd{
	if($oldText ne $field->get_text()){
		$completion->set_model(create_completion_model());
		$oldText=$field->get_text();
	}
	return 1;
}
sub create_completion_model {
	@vals=();
	my $store = Gtk2::ListStore->new (Glib::String::);
	#my $store = Gtk2::ListStore->new (qw/ Glib::String Glib::Object /);
	my $text=$field->get_text();
	my $pref="";
	if(substr($text,0,1) eq "/" or substr($text,0,2) eq "#/"){
		if(substr($text,0,2) eq "#/"){
			$text=substr($text,1);
			$pref="#";
		}
		foreach my $path(glob("$text*")){
			my $str="$pref".decode('UTF-8',$path);
			$store->set ($store->append, 0,$str);
			push(@vals,$str);
		}
		return $store;
	}
	if($text=~/USD/ or $text=~/RUB/ or $text=~/EUR/){
		my $type;
		my $tmpString=$text;
		if($text=~/USD/){
			$type="USD";
			$tmpString=~s/USD//eg;
		}
		if($text=~/RUB/){
			$type="RUB";
			$tmpString=~s/RUB//eg;
		}
		if($text=~/EUR/){
			$type="EUR";
			$tmpString=~s/EUR//eg;
		}
		$tmpString=~tr/,/./;
		my $tmp=$tmpString*1;
		if($tmp>0){
			my $postFix;
			my @data=`allinone s19`;chomp @data;
			$data[0]=~tr/,/./;
			$data[1]=~tr/,/./;
			my $usd=$data[0]*1;
			my $eur=$data[1]*1;
			if($type eq "USD"){
				my $res1=($usd*$tmp);
				my $res2=($usd*$tmp)/$eur;
				($res1,my $res1ost)=split(",",$res1);
				($res2,my $res2ost)=split(",",$res2);
				$res2ost=substr($res2ost,0,2);
				$res2ost=substr($res2ost,0,2);
				my $str1="$tmp USD = $res1.$res2ost RUB";
				my $str2="$tmp USD = $res2.$res2ost EUR";
				$store->set($store->append,0,$str1);
				$store->set($store->append,0,$str2);
				return $store;
			}
			if($type eq "RUB"){
				my $res1=($tmp/$usd);
				my $res2=($tmp/$eur);
				($res1,my $res1ost)=split(",",$res1);
				($res2,my $res2ost)=split(",",$res2);
				$res2ost=substr($res2ost,0,2);
				$res2ost=substr($res2ost,0,2);
				my $str1="$tmp RUB = $res1.$res2ost USD";
				my $str2="$tmp RUB = $res2.$res2ost EUR";
				$store->set($store->append,0,$str1);
				$store->set($store->append,0,$str2);
				return $store;
			}
			if($type eq "EUR"){
				my $res1=($tmp*$eur)/$usd;
				my $res2=($tmp*$eur);
				($res1,my $res1ost)=split(",",$res1);
				($res2,my $res2ost)=split(",",$res2);
				$res2ost=substr($res2ost,0,2);
				$res2ost=substr($res2ost,0,2);
				my $str1="$tmp EUR = $res1.$res2ost USD";
				my $str2="$tmp EUR = $res2.$res2ost RUB";
				$store->set($store->append,0,$str1);
				$store->set($store->append,0,$str2);
				return $store;
			}
		}
	}
	if(substr($text,0,2) eq "c " or substr($text,0,2) eq "с "){
		$pref="c ";
		if(substr($text,0,2) eq "с "){$pref="с ";}
		$text=substr($text,2);
		my ($sday,$smon,$syear)=split("[.]",$text);
		my ($sek,$min,$hour,$day,$mon,$year)=localtime;
		$mon+=1;
		$year+=1900;
		if(!$sday or $sday<=0 or $sday>=31){return $store;}
		if(!$smon or $smon<=0 or $smon>=12){$smon=$mon;}
		if(!$syear or $syear<=1990 or $syear>$year){$syear=$year;}
		my $time = timelocal_nocheck($sek,$min,$hour,$sday,$smon,$syear);
		my $r=timelocal_nocheck($sek,$min,$hour,$day,$mon,$year)-$time;
		if($r<0){return $store;}
		$store->set($store->append,0,"$pref$text прошло $r сек.");
		my $d=int($r/86400);
		if($d>=1){$store->set($store->append,0,"$pref$text прошло $d дн.");}
		my $w=int($d/7);
		if($d>7){
			$d-=$w*7;
			$store->set($store->append,0,"$pref$text прошло $w нед. $d дн.");
		}
		my $m=int(($r/86400)/30);
		if($w>5){$store->set($store->append,0,"$pref$text прошло $m мес.");}
		my $y=int($m/12);
		if($m>12){$store->set($store->append,0,"$pref$text прошло лет: $y");}
		return $store;
	}
	if(substr($text,0,3) eq "до " or substr($text,0,3) eq "lj "){
		$pref="до ";
		if(substr($text,0,3) eq "lj "){$pref="lj ";}
		$text=substr($text,3);
		my ($sday,$smon,$syear)=split("[.]",$text);
		my ($sek,$min,$hour,$day,$mon,$year)=localtime;
		$mon+=1;
		$year+=1900;
		if(!$sday or $sday<=0 or $sday>31){return $store;}
		if(!$smon or $smon<=0 or $smon>12){$smon=$mon;}
		if(!$syear or $syear<=1990 or $syear>$year){$syear=$year;}
		my $time = timelocal_nocheck($sek,$min,$hour,$sday,$smon,$syear);
		my $r=$time-timelocal_nocheck($sek,$min,$hour,$day,$mon,$year);
		if($r<0){return $store;}
		$store->set($store->append,0,"$pref$text осталось $r сек.");
		my $d=int($r/86400);
		if($d>=1){$store->set($store->append,0,"$pref$text осталось $d дн.");}
		my $w=int($d/7);
		if($d>7){
			$d-=$w*7;
			$store->set($store->append,0,"$pref$text осталось $w нед. $d дн.");
		}
		my $m=int(($r/86400)/30);
		if($w>5){$store->set($store->append,0,"$pref$text осталось $m мес.");}
		my $y=int($m/12);
		if($m>12){$store->set($store->append,0,"$pref$text осталось лет: $y");}
		return $store;
	}
	if(substr($text,0,1) eq "^"){$pref="^";$text=substr($text,1);}
	if(substr($text,0,1) eq "!"){$pref="!";$text=substr($text,1);}
	foreach my $path(split(":",$ENV{"PATH"})){
		foreach my $app(glob("$path/$text*")){
			my @tmp=split("/",$app);
			my $str="$pref".decode('UTF-8',pop(@tmp));
			$store->set($store->append,0,"$str");
			push(@vals,$str);
		}
	}
	return $store;
}

