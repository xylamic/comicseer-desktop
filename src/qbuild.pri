#define the configuration for this build
#CONFIG += 32bit

# DEFINE TARGET FILE POSTFIX
win32 {
CONFIG(debug, debug|release) {
        FPOSTFIX = "D"
} else {
        FPOSTFIX = ""
}
}

unix {
FPOSTFIX = ""
}
macos {
FPOSTFIX = ""
}

# COMPILER FLAGS
#win32:QMAKE_CXXFLAGS += -wd4100
#unix:QMAKE_CXXFLAGS += -std=c++0x

#unix:QMAKE_CC = /usr/bin/gcc-4.6
#unix:QMAKE_CXX = /usr/bin/g++-4.6

# DEFINITIONS
DEFINES += BOOST_ALL_NO_LIB
DEFINES += _CRT_SECURE_NO_WARNINGS

# ARCHITECTURE
CONFIG(32bit) {
	win32:ARCH = "win32"
	unix:ARCH = "linux_i386"
} else {
	win32:ARCH = "win64"
	unix:ARCH = "linux_amd64"
}

# INCLUDE PATHS
INCLUDEPATH += "../XComicLibrary"
INCLUDEPATH += "../XCoreLibrary"
INCLUDEPATH += "../Minizip"
INCLUDEPATH += "../Unrar"
INCLUDEPATH += "../../include"
INCLUDEPATH += "/usr/local/Cellar/boost/1.72.0/include"

# LIBRARY DIRECTORIES
LIBS += -L"./../../lib/$$ARCH"
LIBS += -L"/usr/local/Cellar/boost/1.72.0/lib"
#win32 {
#    LIBS += -lUser32
#    LIBS += -lShell32
#    LIBS += -lAdvapi32
#}
#unix {
#    CONFIG(32bit) {
##        LIBS += -L"/usr/lib/i386-linux-gnu"
#    }
#    else {
#        LIBS += -L"/usr/lib/x86_64-linux-gnu"
#    }
#}

HEADERS +=
