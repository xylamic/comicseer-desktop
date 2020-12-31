# STD INCLUDE
include(../qbuild.pri)

# standard C++ library
CONFIG -= qt
CONFIG += staticlib
TEMPLATE = lib

unix:QMAKE_CXXFLAGS += -w

TARGET = XUnrar$$FPOSTFIX

# DESTINATION FOLDER
DESTDIR = "../../lib/$$ARCH"

win32:DEFINES += "WINVER=0x0502"
DEFINES += RARDLL

SOURCES += \
    volume.cpp \
    unpack.cpp \
    unicode.cpp \
    ulinks.cpp \
    timefn.cpp \
    system.cpp \
    strlist.cpp \
    strfn.cpp \
    smallfn.cpp \
    sha1.cpp \
    secpassword.cpp \
    scantree.cpp \
    savepos.cpp \
    rs.cpp \
    rijndael.cpp \
    recvol.cpp \
    rdwrfn.cpp \
    rawread.cpp \
    rarvm.cpp \
    rarpch.cpp \
    rar.cpp \
    pathfn.cpp \
    options.cpp \
    match.cpp \
    list.cpp \
    isnt.cpp \
    global.cpp \
    getbits.cpp \
    find.cpp \
    filestr.cpp \
    filefn.cpp \
    file.cpp \
    filcreat.cpp \
    extract.cpp \
    extinfo.cpp \
    errhnd.cpp \
    encname.cpp \
    dll.cpp \
    crypt.cpp \
    crc.cpp \
    consio.cpp \
    cmddata.cpp \
    arcread.cpp \
    archive.cpp

HEADERS += rar.hpp

