#-------------------------------------------------
#
# Project created by QtCreator 2016-07-31T16:25:08
#
#-------------------------------------------------

QT       += core gui webenginewidgets network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Vik
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

VERSION = 0.1

SOURCES += main.cpp\
        mainwindow.cpp \
    audioframe.cpp \
    loginwidget.cpp \
    messageframe.cpp \
    userframe.cpp \
    dialogframe.cpp \
    updatethread.cpp \
    downloadframe.cpp \
    cache.cpp \
    audioplayer.cpp \
    contentframe.cpp \
    framefactory.cpp

HEADERS  += mainwindow.h \
    audioframe.h \
    loginwidget.h \
    messageframe.h \
    userframe.h \
    dialogframe.h \
    updatethread.h \
    Log.h \
    downloadframe.h \
    cache.h \
    audioplayer.h \
    contentframe.h \
    framefactory.h

FORMS    += mainwindow.ui \
    audioframe.ui \
    loginwidget.ui \
    messageframe.ui \
    userframe.ui \
    dialogframe.ui \
    downloadframe.ui \
    audioplayer.ui

RESOURCES += \
    ux_icons.qrc

INCLUDEPATH += $$PWD/../VKApiCPPLibrary
DEPENDPATH += $$PWD/../VKApiCPPLibrary

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-VKApiCPPLibrary-Desktop_Qt_5_7_0_GCC_64bit-Debug/release/ -lVKApiCPPLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-VKApiCPPLibrary-Desktop_Qt_5_7_0_GCC_64bit-Debug/debug/ -lVKApiCPPLibrary
else:unix: LIBS += -L$$PWD/../build-VKApiCPPLibrary-Desktop_Qt_5_7_0_GCC_64bit-Debug/ -lVKApiCPPLibrary

INCLUDEPATH += $$PWD/../VKApiCPPLibrary
DEPENDPATH += $$PWD/../VKApiCPPLibrary

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-VKApiCPPLibrary-Desktop_Qt_5_7_0_GCC_64bit-Debug/release/libVKApiCPPLibrary.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-VKApiCPPLibrary-Desktop_Qt_5_7_0_GCC_64bit-Debug/debug/libVKApiCPPLibrary.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-VKApiCPPLibrary-Desktop_Qt_5_7_0_GCC_64bit-Debug/release/VKApiCPPLibrary.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-VKApiCPPLibrary-Desktop_Qt_5_7_0_GCC_64bit-Debug/debug/VKApiCPPLibrary.lib
else:unix: PRE_TARGETDEPS += $$PWD/../build-VKApiCPPLibrary-Desktop_Qt_5_7_0_GCC_64bit-Debug/libVKApiCPPLibrary.a
