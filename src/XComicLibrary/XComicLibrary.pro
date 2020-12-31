# STD INCLUDE
include(../qbuild.pri)

# standard C++ library
QT -= core
CONFIG += staticlib
TEMPLATE = lib

TARGET = XComicLibrary$$FPOSTFIX

# DESTINATION FOLDER
DESTDIR = "../../lib/$$ARCH"

SOURCES += \
    ComicUtilities.cpp \
    ComicFile.cpp \
    ComicExceptions.cpp \
    CbzComicFile.cpp \
    CbrComicFile.cpp \
    PdfComicFile.cpp \
    ImageComicFile.cpp \
    ComicFileName.cpp \
    ComicLibraryItem.cpp \
    ComicLibrary.cpp \
    pugixml.cpp \
    XByteArray.cpp \
    XException.cpp \
    XUtilities.cpp \
    XIni.cpp \
    ComicInfo.cpp \
    Bookmarkings.cpp \
    Bookmark.cpp

HEADERS += \
    ComicUtilities.hpp \
    ComicFile.hpp \
    ComicExceptions.hpp \
    CbzComicFile.hpp \
    CbrComicFile.hpp \
    ImageState.hpp \
    ImageView.hpp \
    PdfComicFile.hpp \
    ImageComicFile.hpp \
    ComicFileName.hpp \
    ComicLibraryItem.hpp \
    ComicLibrary.hpp \
    PageNumberLocation.hpp \
    ComicPageTypes.hpp \
    pugiconfig.hpp \
    pugixml.hpp \
    XByteArray.hpp \
    XException.hpp \
    XUtilities.hpp \
    Xylasoft.hpp \
    XIni.hpp \
    ComicInfo.hpp \
    Bookmarkings.hpp \
    Bookmark.hpp

OTHER_FILES += \
    ComicInfo.xsd
