#!/usr/bin/perl
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
use utf8;
use Gtk2 '-init';
use strict;
use warnings;
my $title="PASSWDeditor";
my $file="$ENV{HOME}/passwd.db";
my ($passField,$textbody,$unlockB);
# create window
my $main=Gtk2::Window->new('toplevel');
$main->set_title($title);
$main->set_position("GTK_WIN_POS_CENTER_ALWAYS");
$main->set_size_request(640,480);
$main->set_resizable(0);
####################
my $vbox=Gtk2::VBox->new;
	my $scrollWin=Gtk2::ScrolledWindow->new;
	$scrollWin->set_policy("automatic","automatic");
		$textbody=Gtk2::TextBuffer->new;
		my $TextView=Gtk2::TextView->new;
			$TextView->set_buffer($textbody);
			$TextView->set_editable(1);
	$scrollWin->add($TextView);
$vbox->add($scrollWin);
	# Подсказка
	my $hboxp=Gtk2::HBox->new;
		my $tmpLabel=Gtk2::Label->new;
		$tmpLabel->set_markup("<b>Unlock key:</b>");
		$tmpLabel->set_alignment(0,0.5);
	$hboxp->pack_start($tmpLabel,0,1,0);
		$passField=Gtk2::Entry->new();
		$passField->set_visibility(0);
		$passField->set_invisible_char("#");
		$passField->signal_connect("activate"=>sub{openFile();});
	$hboxp->add($passField);
		$unlockB=Gtk2::Button->new();
		$unlockB->set_image(Gtk2::Image->new_from_stock("gtk-dialog-authentication","menu"));
		$unlockB->signal_connect("clicked"=>sub{openFile();});
	$hboxp->pack_start($unlockB,0,1,0);
$vbox->pack_start($hboxp,0,1,0);
	# Форма выбора файла
	my $hboxsp=Gtk2::HBox->new;
		my $tmpLabel2=Gtk2::Label->new;
		$tmpLabel2->set_markup("<b>File:</b>");
		$tmpLabel2->set_alignment(0,0.5);
	$hboxsp->pack_start($tmpLabel2,0,1,0);
		my $tmpLabel3=Gtk2::Label->new("[$file]");
		$tmpLabel3->set_alignment(0,0.5);
	$hboxsp->add($tmpLabel3);
		my $but=Gtk2::Button->new("SAVE");
		$but->signal_connect("clicked"=>sub{
			my $dialog = Gtk2::Dialog->new ("Save to file",$main,"modal",
                               'gtk-cancel' => 'cancel',
                               'Do it'      => 'ok');
			my $label = Gtk2::Label->new ("Save to file");
    		$dialog->get_content_area ()->add ($label);
			$dialog->set_default_response ('ok');
			my $response = $dialog->run;
			$dialog->destroy;
  			if($response eq 'ok') {
				my $start=$textbody->get_start_iter;
				my $end=$textbody->get_end_iter;
				my $text=$textbody->get_text($start,$end,1);
				$text=xorencode($text,$passField->get_text());
				$text=~s/\x00//eg;
				open FS,">$file";print FS $text;close FS;
				openFile();
  			}
		});
	$hboxsp->pack_start($but,0,1,0);
		my $butc0=Gtk2::Button->new("X");
		$butc0->signal_connect("clicked"=>sub{Gtk2->main_quit;});
	$hboxsp->pack_start($butc0,0,1,0);
$vbox->pack_start($hboxsp,0,1,0);
$main->add($vbox);
# signals
$main->signal_connect("destroy"=>sub{Gtk2->main_quit;});
$main->signal_connect(key_release_event=>sub{
	my ($win,$event)=@_;
	my $key=$event->keyval;
	if($key eq 65307){Gtk2->main_quit;} #ESC
	return 0;
});
# init
$main->show_all;
Gtk2->main;



sub openFile{
	my $text;
	open FS,"<$file";
	while(my $str=<FS>){
		chomp $str;
		my $orig=xordecode($str,$passField->get_text());
		my $finalStr=pack('U0B*',$orig)."\n";
		$text.=$finalStr;
	}
	close FS;
	$text=~s/\x00//eg;
	$textbody->set_text($text);
}
sub xorencode {
    my $string    = shift;
    my $key       = shift;
    my $is_decode = shift;
    my $processed_string = "";
    my $string_unpacked = $is_decode ? $string : unpack('U0B*', $string);
    my $key_unpacked = unpack('U0B*', $key);
    my $key_length = length $key_unpacked;
    for my $part (unpack("(a$key_length)*", $string_unpacked)) {
        my $a = pack('B*', $part);
        my $b = pack('B*', $key_unpacked);
        my $c = $a ^ $b;
        my $unpacked_c = unpack('B*', $c);
        $processed_string .= $unpacked_c;
    } 
    return $processed_string;
}
sub xordecode { $_[2] = 1; goto &xorencode; }
