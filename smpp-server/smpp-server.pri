QMAKE_CXXFLAGS += -I$$PWD

include($$PWD/smppcxx/smppcxx.pri)

HEADERS += \
    $$PWD/smpp-server/smppserver.h \
    $$PWD/smpp-server/smppsession.h \
    $$PWD/qtopia/qgsmcodec.h \
    $$PWD/smpp-server/interfaces/ismsgateway.h

SOURCES += \
    $$PWD/smpp-server/smppserver.cpp \
    $$PWD/smpp-server/smppsession.cpp \
    $$PWD/qtopia/qgsmcodec.cpp

