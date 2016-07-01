#-------------------------------------------------
#
# Project created by QtCreator 2011-11-16T19:32:37
#
#-------------------------------------------------

QT       += core gui

INCLUDEPATH   += C:/qextserialport/src
QMAKE_LIBDIR  += C:/qextserialport-build-desktop/src/build

TARGET = QanaQit
TEMPLATE = app

CONFIG(debug, debug|release):LIBS  += -lqextserialportd1
else:LIBS  += -lqextserialport1
win32:LIBS += -lsetupapi

CONFIG += thread

SOURCES += main.cpp\
        qanaqitmaindlg.cpp \
    LoggerTime.cpp \
    qanameasurement.cpp \
    relaysettings.cpp \
    Global.cpp \
    canakitcontroller.cpp \
    experimentmanager.cpp

HEADERS  += qanaqitmaindlg.h \
    LoggerTime.h \
    qanameasurement.h \
    Global.h \
    relaysettings.h \
    canakitcontroller.h \
    experimentmanager.h

FORMS    += qanaqitmaindlg.ui













