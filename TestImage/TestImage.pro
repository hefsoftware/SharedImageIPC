QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    ..\SharedMem\sharedmem.c \
    ..\SharedMem\arch\sharedmemwin.c \
    ..\SharedImage\sharedimage.c \
    hfsharedimage.cpp \
    imagecanvas.cpp \
    imageprovider.cpp \
    imageviewer.cpp \
    imagewidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ..\SharedMem\sharedmem.h \
    ..\SharedMem\arch\sharedmemarch.h \
    ..\SharedMem\internal\sharedmeminternal.h \
    ..\SharedImage\sharedimage.h \
    hfsharedimage.h \
    imagecanvas.h \
    imageprovider.h \
    imageviewer.h \
    imagewidget.h \
    mainwindow.h

FORMS += \
    imageprovider.ui \
    imageviewer.ui \
    mainwindow.ui

win32:DEFINES += SHAREDMEM_WIN32

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SharedImage/release/ -lSharedImage
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SharedImage/debug/ -lSharedImage
#else:unix: LIBS += -L$$OUT_PWD/../LibraryImage/ -lSharedImage
INCLUDEPATH += $$PWD/../SharedMem
INCLUDEPATH += $$PWD/../SharedImage
