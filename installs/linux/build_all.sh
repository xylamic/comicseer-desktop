#!/bin/sh

echo Validating parameters...
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

echo Setting variables...
QMake=/usr/bin/qmake-qt4
LRelease=/usr/bin/lrelease-qt4
LUpdate=/usr/bin/lupdate-qt4
Arch=$1
Version=$2
BaseDir=/media/xylamic/Data/Development/ComicSeer/trunk
DebDir=/media/xylamic/Data/Development/ComicSeer/trunk/installs/deb
UnrarBuildDir=build-Unrar-$Arch-Dist
MinizipBuildDir=build-Minzip-$Arch-Dist
ComicLibBuildDir=build-ComicLibrary-$Arch-Dist
ComicDeskBuildDir=build-ComicDesktop-$Arch-Dist

echo "Setting architecture for ${1}"...
if [ "$1" = "amd64" ]; then
	QMakeSpec=linux-g++-64
	sed 's/CONFIG += 32bit/CONFIG -= 32bit/' $BaseDir/src/qbuild.pri > $BaseDir/src/qbuild.pri.new
	mv $BaseDir/src/qbuild.pri.new $BaseDir/src/qbuild.pri
else
	QMakeSpec=linux-g++-32
	sed 's/CONFIG -= 32bit/CONFIG += 32bit/' $BaseDir/src/qbuild.pri > $BaseDir/src/qbuild.pri.new
	mv $BaseDir/src/qbuild.pri.new $BaseDir/src/qbuild.pri
fi

echo Cleaning Unrar...
rm -rf $BaseDir/src/$UnrarBuildDir
rm -f $BaseDir/lib/linux_$Arch/libXUnrar.a
echo Cleaning Minizip...
rm -rf $BaseDir/src/$MinizipBuildDir
rm -f $BaseDir/lib/linux_$Arch/libXMinizip.a
echo Cleaning ComicLibrary...
rm -rf $BaseDir/src/$ComicLibBuildDir
rm -f $BaseDir/lib/linux_$Arch/libXComicLibrary.a
echo Cleaning ComicSeer...
rm -rf $BaseDir/src/$ComicDeskBuildDir
rm -f $BaseDir/bin/linux_$Arch/comicseer

echo Making new build directories...
mkdir $BaseDir/src/$UnrarBuildDir
mkdir $BaseDir/src/$MinizipBuildDir
mkdir $BaseDir/src/$ComicLibBuildDir
mkdir $BaseDir/src/$ComicDeskBuildDir

echo Running Unrar qmake...
cd $BaseDir/src/$UnrarBuildDir
$QMake ../Unrar/Unrar.pro -r -spec $QMakeSpec
rc=$?
if [ $rc != 0 ] ; then
	echo Failed to qmake with error $rc.
    exit $rc
else
	echo "QMaked Unrar"
fi

echo Building Unrar...
make
rc=$?
if [ $rc != 0 ] ; then
   echo Failed to make with error $rc.
   exit $rc
else
	echo "Built Unrar"
fi

echo Running Minizip qmake...
cd $BaseDir/src/$MinizipBuildDir
$QMake ../Minizip/Minizip.pro -r -spec $QMakeSpec
rc=$?
if [ $rc != 0 ] ; then
	echo Failed to qmake with error $rc.
    exit $rc
else
	echo "Qmaked Minizip"
fi

echo Building Minizip...
make
rc=$?
if [ $rc != 0 ] ; then
   echo Failed to make with error $rc.
   exit $rc
else
	echo "Built Minizip"
fi

echo Running XComicLibrary qmake...
cd $BaseDir/src/$ComicLibBuildDir
$QMake ../XComicLibrary/XComicLibrary.pro -r -spec $QMakeSpec
rc=$?
if [ $rc != 0 ] ; then
	echo Failed to qmake with error $rc.
    exit $rc
else
	echo "Qmaked XComicLibrary"
fi

echo Building XComicLibrary...
make
rc=$?
if [ $rc != 0 ] ; then
   echo Failed to make with error $rc.
   exit $rc
else
	echo "Built XComicLibrary"
fi

echo Running ComicDesktop qmake...
cd $BaseDir/src/$ComicDeskBuildDir
$QMake ../ComicDesktop/ComicDesktop.pro -r -spec $QMakeSpec
rc=$?
if [ $rc != 0 ] ; then
	echo Failed to qmake with error $rc.
    exit $rc
else
	echo "Qmaked ComicDesktop"
fi

echo Building ComicDesktop...
make
rc=$?
if [ $rc != 0 ] ; then
   echo Failed to make with error $rc.
   exit $rc
else
	echo "Built ComicDesktop"
fi

echo Updating translations...
cd $BaseDir/src/ComicDesktop
$LUpdate ComicDesktop.pro
rc=$?
if [ $rc != 0 ] ; then
   echo Failed to update translations with error $rc.
   exit $rc
else
	echo "Updated translations"
fi

echo Compiling translations...
cd $BaseDir/src/ComicDesktop
$LRelease ComicDesktop.pro
rc=$?
if [ $rc != 0 ] ; then
   echo Failed to compile translations with error $rc.
   exit $rc
else
	echo "Compiled translations"
fi

echo Building DEB...
cd $DebDir
sudo ../linux/build_deb.sh $Arch $Version
rc=$?
if [ $rc != 0 ] ; then
   echo Failed to build package with error $rc.
   exit $rc
else
	echo "Built package"
fi

echo COMPLETED SUCCESSFULLY
