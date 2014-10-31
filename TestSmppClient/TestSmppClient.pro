#-------------------------------------------------
#
# Project created by QtCreator 2014-10-28T09:23:50
#
#-------------------------------------------------

include($$PWD/../smpp-client/smpp-client.pri)

QMAKE_CXXFLAGS += -std=gnu++11

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestSmppClient
TEMPLATE = app


SOURCES += main.cpp\
        testsmppclientmainwindow.cpp

HEADERS  += testsmppclientmainwindow.h

FORMS    += testsmppclientmainwindow.ui
