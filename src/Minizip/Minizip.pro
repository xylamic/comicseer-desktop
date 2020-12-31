# STD INCLUDE
include(../qbuild.pri)

# standard C++ library
CONFIG -= qt
CONFIG += staticlib
TEMPLATE = lib

#unix:QMAKE_CXXFLAGS += -w

TARGET = XMinizip$$FPOSTFIX

# DESTINATION FOLDER
DESTDIR = "../../lib/$$ARCH"

DEFINES -= _GLOBAL_USING

SOURCES += \
    XZip.cpp \
    unzip.c \
    mztools.c \
    miniunz.c \
    FileFormatMinizipException.cpp \
    fopen.cpp \
    ioapi.c

HEADERS += \
    XZip.hpp \
    unzip.h \
    mztools.h \
    ioapi.h \
    FileFormatMinizipException.h \
    crypt.h

CONFIG(32bit) {
    win32:DESTDIR = "../../lib/win32"
    unix:DESTDIR = "../../lib/linux_i386"
}
else {
    win32:DESTDIR = "../../lib/win64"
    unix:DESTDIR = "../../lib/linux_amd64"
}
