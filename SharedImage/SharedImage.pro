QT -= gui

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    sharedimage.c \
    ../SharedMem/sharedmem.c \
    ../SharedMem/arch/sharedmemwin.c

HEADERS += \
    sharedimage.h

win32:DEFINES += SHAREDMEM_WIN32

INCLUDEPATH += $$PWD/../SharedMem
