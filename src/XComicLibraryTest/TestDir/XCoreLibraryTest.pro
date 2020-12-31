include(../qbuild.pri)

# QT GUI application
TEMPLATE = app
CONFIG += console
CONFIG -= qt

TARGET = XCoreLibraryTest$$FPOSTFIX

unix:QMAKE_CXXFLAGS += -Wsign-compare

# DESTINATION FOLDER
DESTDIR = "../../bin/$$ARCH"

win32 {
    Debug:LIBS += -lXCoreLibraryD \
                    -lgtestD
    Release:LIBS += -lXCoreLibrary \
                    -lgtest
} else {
    LIBS += -lXCoreLibrary \
                    -lgtest \
                    -lpthread
}

win32 {
    PRE_TARGETDEPS += "../../lib/$$ARCH/XCoreLibrary$${FPOSTFIX}.lib"
} else {
    PRE_TARGETDEPS += "../../lib/$$ARCH/libXCoreLibrary$${FPOSTFIX}.lib"
}

SOURCES += main.cpp \
    XUuidTest.cpp \
    XIniTest.cpp \
    XUtilitiesTest.cpp

OTHER_FILES +=
