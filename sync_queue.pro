TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread
SOURCES += main.cpp

HEADERS += \
    sync_queue.h
