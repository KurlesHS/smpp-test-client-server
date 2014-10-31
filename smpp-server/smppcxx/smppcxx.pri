QMAKE_CXXFLAGS += -I$$PWD/../smppcxx

OTHER_FILES += \
    $$PWD/lgpl.txt

HEADERS += \
    $$PWD/alert_notification.hpp \
    $$PWD/aux_types.hpp \
    $$PWD/bind_receiver.hpp \
    $$PWD/bind_receiver_resp.hpp \
    $$PWD/bind_transceiver.hpp \
    $$PWD/bind_transceiver_resp.hpp \
    $$PWD/bind_transmitter.hpp \
    $$PWD/bind_transmitter_resp.hpp \
    $$PWD/broadcast_sm.hpp \
    $$PWD/broadcast_sm_resp.hpp \
    $$PWD/buffer.hpp \
    $$PWD/cancel_broadcast_sm.hpp \
    $$PWD/cancel_broadcast_sm_resp.hpp \
    $$PWD/cancel_sm.hpp \
    $$PWD/cancel_sm_resp.hpp \
    $$PWD/command_id.hpp \
    $$PWD/command_length.hpp \
    $$PWD/command_status.hpp \
    $$PWD/data_sm.hpp \
    $$PWD/data_sm_resp.hpp \
    $$PWD/deliver_sm.hpp \
    $$PWD/deliver_sm_resp.hpp \
    $$PWD/enquire_link.hpp \
    $$PWD/enquire_link_resp.hpp \
    $$PWD/error.hpp \
    $$PWD/generic_nack.hpp \
    $$PWD/header.hpp \
    $$PWD/outbind.hpp \
    $$PWD/query_broadcast_sm.hpp \
    $$PWD/query_broadcast_sm_resp.hpp \
    $$PWD/query_sm.hpp \
    $$PWD/query_sm_resp.hpp \
    $$PWD/replace_sm.hpp \
    $$PWD/replace_sm_resp.hpp \
    $$PWD/sequence_number.hpp \
    $$PWD/smpp.hpp \
    $$PWD/submit_multi.hpp \
    $$PWD/submit_multi_resp.hpp \
    $$PWD/submit_sm.hpp \
    $$PWD/submit_sm_resp.hpp \
    $$PWD/tlv.hpp \
    $$PWD/unbind.hpp \
    $$PWD/unbind_resp.hpp

SOURCES += \
    $$PWD/alert_notification.cpp \
    $$PWD/aux_types.cpp \
    $$PWD/bind_receiver.cpp \
    $$PWD/bind_receiver_resp.cpp \
    $$PWD/bind_transceiver.cpp \
    $$PWD/bind_transceiver_resp.cpp \
    $$PWD/bind_transmitter.cpp \
    $$PWD/bind_transmitter_resp.cpp \
    $$PWD/broadcast_sm.cpp \
    $$PWD/broadcast_sm_resp.cpp \
    $$PWD/buffer.cpp \
    $$PWD/cancel_broadcast_sm.cpp \
    $$PWD/cancel_broadcast_sm_resp.cpp \
    $$PWD/cancel_sm.cpp \
    $$PWD/cancel_sm_resp.cpp \
    $$PWD/data_sm.cpp \
    $$PWD/data_sm_resp.cpp \
    $$PWD/deliver_sm.cpp \
    $$PWD/deliver_sm_resp.cpp \
    $$PWD/enquire_link.cpp \
    $$PWD/enquire_link_resp.cpp \
    $$PWD/error.cpp \
    $$PWD/generic_nack.cpp \
    $$PWD/header.cpp \
    $$PWD/outbind.cpp \
    $$PWD/query_broadcast_sm.cpp \
    $$PWD/query_broadcast_sm_resp.cpp \
    $$PWD/query_sm.cpp \
    $$PWD/query_sm_resp.cpp \
    $$PWD/replace_sm.cpp \
    $$PWD/replace_sm_resp.cpp \
    $$PWD/submit_multi.cpp \
    $$PWD/submit_multi_resp.cpp \
    $$PWD/submit_sm.cpp \
    $$PWD/submit_sm_resp.cpp \
    $$PWD/unbind.cpp \
    $$PWD/unbind_resp.cpp
