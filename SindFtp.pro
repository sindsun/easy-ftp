TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread libwsock32 libws2_32

SOURCES += \
        common.cpp \
        index.cpp \
        main.cpp

HEADERS += \
    common.h \
    index.h
