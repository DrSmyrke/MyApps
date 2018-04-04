#!/bin/bash
set -o nounset -o errexit

DIR=dists/stable
rm -rf $DIR
mkdir -p $DIR

echo "Archive: stable" > $DIR/Release
echo "Suite: stable" >> $DIR/Release
echo "Components: main" >> $DIR/Release
echo "Origin: Jura Prokofev repo" >> $DIR/Release
echo "Architectures: i386 amd64 source" >> $DIR/Release

mkdir -p $DIR/main/binary-amd64
mkdir -p $DIR/main/binary-i386

dpkg-scanpackages --arch amd64 pool /dev/null > $DIR/main/binary-amd64/Packages
gzip -9c <$DIR/main/binary-amd64/Packages >$DIR/main/binary-amd64/Packages.gz
bzip2 -9c <$DIR/main/binary-amd64/Packages >$DIR/main/binary-amd64/Packages.bz2

dpkg-scanpackages --arch i386 pool /dev/null > $DIR/main/binary-i386/Packages
gzip -9c <$DIR/main/binary-i386/Packages >$DIR/main/binary-i386/Packages.gz
bzip2 -9c <$DIR/main/binary-i386/Packages >$DIR/main/binary-i386/Packages.bz2

apt-ftparchive release $DIR >>$DIR/Release

gpg --armor -o $DIR/Release.gpg -sb $DIR/Release
gpg --clearsign --digest-algo SHA512 -o $DIR/InRelease $DIR/Release



#cd $DIR
#gpg --clearsign -o InRelease
#gpg -abs -o $DIR/Release.gpg $DIR/Release

#dpkg-scansources main /dev/null > $dir/Sources
#gzip -9c <$dir/Sources >$dir/Sources.gz
#bzip2 -9c <$dir/Sources >$dir/Sources.bz2
