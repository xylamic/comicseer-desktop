@echo OFF

echo Setting paths...
SET PATH=C:\Qt\4.8.6\bin;C:\Qt\qtcreator-3.1.0\bin;%PATH%
CALL "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
SET BaseDir=C:\Users\XYLAMIC\Development\SVN\ComicSeer\trunk
SET UnrarBuildDir=build-Unrar-Dist
SET MinizipBuildDir=build-Minzip-Dist
SET ComicLibBuildDir=build-ComicLibrary-Dist
SET ComicDeskBuildDir=build-ComicDesktop-Dist

echo Cleaning Unrar...
RMDIR /S /Q %BaseDir%\src\%UnrarBuildDir%
DEL /Q %BaseDir%\lib\win32\XUnrar.lib
echo Cleaning Minizip...
RMDIR /S /Q %BaseDir%\src\%MinizipBuildDir%
DEL /Q %BaseDir%\lib\win32\XMinizip.lib
echo Cleaning ComicLibrary...
RMDIR /S /Q %BaseDir%\src\%ComicLibBuildDir%
DEL /Q %BaseDir%\lib\win32\XComicLibrary.lib
echo Cleaning ComicSeer...
RMDIR /S /Q %BaseDir%\src\%ComicDeskBuildDir%
DEL /Q %BaseDir%\bin\win32\comicseer.exe

echo Making new build directories...
MKDIR %BaseDir%\src\%UnrarBuildDir%
MKDIR %BaseDir%\src\%MinizipBuildDir%
MKDIR %BaseDir%\src\%ComicLibBuildDir%
MKDIR %BaseDir%\src\%ComicDeskBuildDir%

echo Running Unrar qmake...
cd %BaseDir%\src\%UnrarBuildDir%
qmake.exe %BaseDir%\src\Unrar\Unrar.pro -r -spec win32-msvc2010 "CONFIG+=release"
if errorlevel 1 (
   echo Failed to qmake with error %errorlevel%.
   exit /b %errorlevel%
)

echo Building Unrar...
jom.exe
if errorlevel 1 (
   echo Failed to make with error %errorlevel%.
   exit /b %errorlevel%
)

echo Running Minizip qmake...
cd %BaseDir%\src\%MinizipBuildDir%
qmake.exe %BaseDir%\src\Minizip\Minizip.pro -r -spec win32-msvc2010 "CONFIG+=release"
if errorlevel 1 (
   echo Failed to qmake with error %errorlevel%.
   exit /b %errorlevel%
)

echo Building Minzip...
jom.exe
if errorlevel 1 (
   echo Failed to make with error %errorlevel%.
   exit /b %errorlevel%
)

echo Running ComicLibrary qmake...
cd %BaseDir%\src\%ComicLibBuildDir%
qmake.exe %BaseDir%\src\XComicLibrary\XComicLibrary.pro -r -spec win32-msvc2010 "CONFIG+=release"
if errorlevel 1 (
   echo Failed to qmake with error %errorlevel%.
   exit /b %errorlevel%
)

echo Building ComicLibrary...
jom.exe
if errorlevel 1 (
   echo Failed to make with error %errorlevel%.
   exit /b %errorlevel%
)

echo Running ComicSeer qmake...
cd %BaseDir%\src\%ComicDeskBuildDir%
qmake.exe %BaseDir%\src\ComicDesktop\ComicDesktop.pro -r -spec win32-msvc2010 "CONFIG+=release"
if errorlevel 1 (
   echo Failed to qmake with error %errorlevel%.
   exit /b %errorlevel%
)

echo Building ComicSeer...
jom.exe
if errorlevel 1 (
   echo Failed to make with error %errorlevel%.
   exit /b %errorlevel%
)

echo Updating translations...
cd %BaseDir%\src\ComicDesktop
lupdate.exe ComicDesktop.pro

echo Compiling translations...
cd %BaseDir%\src\ComicDesktop
lrelease.exe ComicDesktop.pro

echo Building Install...
cd %BaseDir%\installs\win32\
devenv.exe %BaseDir%\installs\win32\WindowsDesktop.vdproj /Rebuild
if errorlevel 1 (
   echo Failed to make install with error %errorlevel%.
   exit /b %errorlevel%
)

echo COMPLETED SUCCESSFULLY