#!/bin/sh

# Validating parameters
if [ -z "$1" ]; then
	echo "usage: $0 <i386|amd64>"
	exit 1
elif [ "$1" != "amd64" ]; then
	if [ "$1" != "i386" ]; then
		echo "usage: $0 <i386|amd64>"
		exit 1
	fi
fi

if [ -z "$2" ]; then
	echo "usage: $0 <i386|amd64> <version>"
	exit 1
fi

# set the architecture and version from the parameters
ARCH=$1
VER=$2

# create the build and dist directories if needed
if [ ! -d ../build ]; then
	mkdir ../build
fi
if [ ! -d ../dist ]; then
	mkdir ../dist
fi

# set the build and dist directories
BUILD=../build/$ARCH
DIST=../dist/$ARCH

echo "Cleaning and Creating Build/Dist directories..."

rm -rf $BUILD
mkdir $BUILD

rm -rf $DIST
mkdir $DIST

echo "Copying Variable Files..."

cp -v ../../changelog/changelog usr/share/doc/comicseer/
cp -v ../../changelog/changelog.Debian usr/share/doc/comicseer/
cp -v ../../src/ComicDesktop/comicseer_sample.ts usr/share/doc/comicseer/
cp -v ../../bin/linux_$ARCH/comicseer usr/bin/

echo Stripping binaries...

strip usr/bin/comicseer

echo GZipping appropriate files...

cp -v usr/share/man/man1/comicseer.1 usr/share/man/man1/comicseer.1.copy

rm -v usr/share/doc/comicseer/*.gz
rm -v usr/share/man/man1/*.gz

gzip -9 usr/share/doc/comicseer/changelog
gzip -9 usr/share/doc/comicseer/changelog.Debian
gzip -9 usr/share/man/man1/comicseer.1

mv -v usr/share/man/man1/comicseer.1.copy usr/share/man/man1/comicseer.1

echo "Modify control file for architecture and version..."
if [ "$ARCH" = "i386" ]; then
	sed 's/Architecture: amd64/Architecture: i386/' DEBIAN/control > DEBIAN/control_new
else
	sed 's/Architecture: i386/Architecture: amd64/' DEBIAN/control > DEBIAN/control_new
fi
python ../linux/set_control_version.py DEBIAN/control_new $VER
if [ "$?" -ne "0" ]; then
	exit 1
fi
mv DEBIAN/control_new DEBIAN/control

echo Creating md5sums...

md5sum usr/bin/comicseer usr/share/applications/comicseer.desktop usr/share/doc/comicseer/changelog.gz usr/share/doc/comicseer/changelog.Debian.gz usr/share/doc/comicseer/comicseer_sample.ts usr/share/doc/comicseer/copyright usr/share/man/man1/comicseer.1.gz usr/share/menu/comicseer usr/share/mime/packages/comicseer.xml usr/share/pixmaps/comicseer.png usr/share/pixmaps/comicseer.xpm > DEBIAN/md5sums

if [ "$?" -ne "0" ]; then
	exit 1
fi

chmod 644 DEBIAN/md5sums
chown 0 DEBIAN/md5sums
chgrp 0 DEBIAN/md5sums

echo Copying hierarchy...

cp -v -R . $BUILD
rm $BUILD/usr/share/man/man1/comicseer.1

echo Setting permissions...

chmod 755 $BUILD/DEBIAN/postinst
chmod 755 $BUILD/DEBIAN/postrm
chmod 755 $BUILD/usr/bin/comicseer
chmod 644 $BUILD/usr/share/doc/comicseer/changelog.gz
chmod 644 $BUILD/usr/share/doc/comicseer/changelog.Debian.gz

echo Building DEB...

dpkg-deb --build $BUILD $DIST/comicseer_${VER}_${ARCH}.deb
if [ "$?" -ne "0" ]; then
	exit 1
fi

echo Validating DEB...

lintian $DIST/comicseer_${VER}_${ARCH}.deb
if [ "$?" -ne "0" ]; then
	exit 1
fi

echo Copying files to dist directory...
cp -v ../../bin/linux_$ARCH/comicseer $DIST
cp -v ../../doc/ComicSeer/Dist/Readme.txt $DIST
cp -v ../../doc/ComicSeer/Images/XylasoftComicsLogo.png $DIST
cp -v ../../src/ComicDesktop/comicseer_sample.ts $DIST
cp -v ../../doc/copyright $DIST
cp -v ../../doc/qtcopyright $DIST

ZIPFILE=ComicSeer_Linux_${ARCH}_v${VER}.zip
echo Zipping files to ${ZIPFILE} for non-deb install...
cd $DIST
zip $ZIPFILE comicseer Readme.txt XylasoftComicsLogo.png comicseer_sample.ts copyright qtcopyright

if [ "$?" -ne "0" ]; then
	exit 1
fi

# echo Skipping signing of DEB due to missing secret key right now...
# echo Signing DEB
# dpkg-sig --sign builder comicseer_${VER}_${ARCH}.deb
# if [ "$?" -ne "0" ]; then
# 	exit 1
# fi

echo Completed packaging process.
