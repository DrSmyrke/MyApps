#!/bin/bash
wget http://drsmyrke-home.pskovline.ru/deb/keyFile
sudo apt-key add keyFile
rm keyFile
echo 'deb http://drsmyrke-home.pskovline.ru/deb stable main' | sudo tee /etc/apt/sources.list.d/drsmyrke.list
sudo apt-get update
