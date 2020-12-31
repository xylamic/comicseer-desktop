# STD INCLUDE
include(../qbuild.pri)

# QT GUI application
TEMPLATE = app
CONFIG += console
CONFIG -= qt

TARGET = XComicLibraryTest$$FPOSTFIX

# DESTINATION FOLDER
DESTDIR = "../../bin/$$ARCH"

unix:QMAKE_CXXFLAGS += -Wsign-compare

win32 {
    Debug:LIBS +=   -lXComicLibraryD \
                    -lXMinizipD \
                    -lXUnrarD \
                    -lgtestD \
                    -llibboost_regex-vc140-mt-gd-1_60
    Release:LIBS += -lXComicLibrary \
                    -lXMinizip \
                    -lXUnrar \
                    -lgtest \
                    -llibboost_regex-vc140-mt-1_60
} else {
    LIBS +=         -lXComicLibrary \
                    -lXMinizip \
                    -lXUnrar \
                    -lgtest \
                    -lboost_regex \
                    -ldl
}

win32 {
    PRE_TARGETDEPS += "../../lib/$$ARCH/XMinizip$${FPOSTFIX}.lib"
    PRE_TARGETDEPS += "../../lib/$$ARCH/XUnrar$${FPOSTFIX}.lib"
    PRE_TARGETDEPS += "../../lib/$$ARCH/XComicLibrary$${FPOSTFIX}.lib"
} else {
    PRE_TARGETDEPS += "../../lib/$$ARCH/libXMinizip$${FPOSTFIX}.a"
    PRE_TARGETDEPS += "../../lib/$$ARCH/libXUnrar$${FPOSTFIX}.a"
    PRE_TARGETDEPS += "../../lib/$$ARCH/libXComicLibrary$${FPOSTFIX}.a"
}

win32:LIBS += -lzlib
unix:LIBS += -lz -lpthread

DEPENDPATH += .

SOURCES += main.cpp \
    ZipTest.cpp \
    ComicInformationTest.cpp \
    CbzComicFileTest.cpp \
    CbzComicFileMultilevelTest.cpp \
    CbrComicFileTest.cpp \
    CbrComicFileMultilevelTest.cpp \
    CbrComicFileTestJap.cpp \
    CbzComicFileTestJap.cpp \
    ComicUtilitiesTest.cpp \
    PdfComicFileTest.cpp \
    LibraryContainerTest.cpp \
    ComicFileNameTest.cpp \
    XUtilitiesTest.cpp \
    XIniTest.cpp \
    BookmarksTest.cpp
