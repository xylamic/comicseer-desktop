# STD INCLUDE
include(../qbuild.pri)

# QT GUI application
QT += core gui network widgets
TEMPLATE = app

# set the target file name
TARGET = comicseer$$FPOSTFIX

# DESTINATION FOLDER
DESTDIR = "../../bin/$$ARCH"

# DEFINITIONS
DEFINES += QT_DLL QT_NETWORK_LIB

# DEPENDENT USER LIBRARIES
win32 {
    Debug:LIBS +=   -lXComicLibraryD \
                    -lXMinizipD \
                    -lXUnrarD \
                    -llibboost_regex-vc140-mt-gd-1_60
    Release:LIBS += -lXComicLibrary \
                    -lXMinizip \
                    -lXUnrar \
                    -llibboost_regex-vc140-mt-1_60
} else {
    LIBS +=         -lXComicLibrary \
                    -lXMinizip \
                    -lXUnrar \
                    -lboost_regex
                    # force boost to link statically
                    #-Wl,-Bstatic -lboost_regex -Wl,-Bdynamic
}

win32 {
    PRE_TARGETDEPS += "../../lib/$$ARCH/XComicLibrary$${FPOSTFIX}.lib"
    PRE_TARGETDEPS += "../../lib/$$ARCH/XMinizip$${FPOSTFIX}.lib"
    PRE_TARGETDEPS += "../../lib/$$ARCH/XUnrar$${FPOSTFIX}.lib"
} else {
    PRE_TARGETDEPS += "../../lib/$$ARCH/libXComicLibrary$${FPOSTFIX}.a"
    PRE_TARGETDEPS += "../../lib/$$ARCH/libXMinizip$${FPOSTFIX}.a"
    PRE_TARGETDEPS += "../../lib/$$ARCH/libXUnrar$${FPOSTFIX}.a"
}

# DEPENDENT SYSTEM LIBRARIES
win32:LIBS += -lzlib
unix:LIBS += -lz

# PROJECT FILES
HEADERS += \
    ComicDesktop.h \
    LibraryViewContent.h \
    ComicDockWidget.h \
    ComicView.h \
    ComicLaunchEvent.h \
    ComicViewWidget.h \
    ComicImageReader.h \
    BrowserContent.h \
    ComicSelectionTabWidget.h \
    LibraryChangedEvent.h \
    PreviewLabel.h \
    RecentComicMenu.h \
    Options.h \
    CommandEnums.h \
    ProgressDialog.h \
    DescribedPreviewLabel.h \
    ColorPicker.h \
    TestDialog.h \
    UpdateButton.h \
    AppQuickstartBrowser.h \
    HomePage.h \
    qtsinglecoreapplication.h \
    qtsingleapplication.h \
    qtlockedfile.h \
    qtlocalpeer.h \
    ComicState.h \
    ComicInfoViewer.h \
    ComicUtils.h \
    AboutView.h \
    BookmarkViewContent.h \
    BusyIndicator.h \
    ComicAppNotification.h

SOURCES += BrowserContent.cpp \
    ComicDesktop.cpp \
    ComicImageReader.cpp \
    ComicLaunchEvent.cpp \
    ComicSelectionTabWidget.cpp \
    ComicView.cpp \
    ComicViewWidget.cpp \
    LibraryChangedEvent.cpp \
    ComicDockWidget.cpp \
    LibraryViewContent.cpp \
    main.cpp \
    PreviewLabel.cpp \
    RecentComicMenu.cpp \
    Options.cpp \
    ProgressDialog.cpp \
    DescribedPreviewLabel.cpp \
    ColorPicker.cpp \
    TestDialog.cpp \
    UpdateButton.cpp \
    AppQuickstartBrowser.cpp \
    HomePage.cpp \
    qtsinglecoreapplication.cpp \
    qtsingleapplication.cpp \
    qtlockedfile_win.cpp \
    qtlockedfile_unix.cpp \
    qtlockedfile.cpp \
    qtlocalpeer.cpp \
    ComicState.cpp \
    ComicInfoViewer.cpp \
    ComicUtils.cpp \
    AboutView.cpp \
    BookmarkViewContent.cpp \
    BusyIndicator.cpp \
    ComicAppNotification.cpp
FORMS += ComicDesktop.ui \
    LibraryView.ui \
    ComicViewer.ui \
    Browser.ui \
    Options.ui \
    DescribedPreviewLabel.ui \
    TestDialog.ui \
    HomePage.ui \
    ComicInfoViewer.ui \
    AboutView.ui \
    BookmarkViewContent.ui \
    BusyIndicator.ui \
    ComicAppNotification.ui
RESOURCES += ComicDesktop.qrc

win32:RC_FILE = WinResource.rc

OTHER_FILES += \
    style.qss \
    WinResource.rc

# TRANSLATION
TRANSLATIONS += comicseer_sample.ts
TRANSLATIONS += comicseer_cs_CZ.ts
TRANSLATIONS += comicseer_fr_FR.ts
TRANSLATIONS += comicseer_ru_RU.ts
