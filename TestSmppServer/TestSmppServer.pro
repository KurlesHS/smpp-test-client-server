#-------------------------------------------------
#
# Project created by QtCreator 2014-10-28T08:02:29
#
#-------------------------------------------------

include($$PWD/../smpp-server/smpp-server.pri)

QMAKE_CXXFLAGS += -std=gnu++11

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestSmppServer
TEMPLATE = app


SOURCES += main.cpp\
        testsmppservermainwindow.cpp \
    smsgateway.cpp

HEADERS  += testsmppservermainwindow.h \
    smsgateway.h

FORMS    += testsmppservermainwindow.ui
