QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

SOURCES += \
        main.cpp

win32:DEFINES += SHAREDMEM_WIN32

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SharedMem/release/ -lSharedMem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SharedMem/debug/ -lSharedMem
else:unix: LIBS += -L$$OUT_PWD/../SharedMem/ -lSharedMem

INCLUDEPATH += $$PWD/../SharedMem
