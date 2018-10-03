#!/bin/bash
wget http://drsmyrke.ru/deb/pubring.gpg
sudo apt-key add pubring.gpg
rm pubring.gpg
echo 'deb http://drsmyrke.ru/deb stable main' | sudo tee /etc/apt/sources.list.d/drsmyrke.list
sudo apt-get update
