CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    arch/sharedmemwin.c \
    sharedmem.c

HEADERS += \
    arch/sharedmemarch.h \
    internal/sharedmeminternal.h \
    sharedmem.h

win32:DEFINES += SHAREDMEM_WIN32
