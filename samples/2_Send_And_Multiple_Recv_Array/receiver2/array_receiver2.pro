TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = array_receiver2

DESTDIR += $$PWD/../../../bin

INCLUDEPATH +=  \
        $$PWD/../../../include

SOURCES += \
        main.cpp

win32 {
    DEFINES += _WINDOWS_ _AMD64_

    LIBS += -L$$PWD/../../../platform/win10_x64_vc15/lib \
            -limpro_core1 \
            -limpro_alljoyn1 \
            -lopencv_world343
}
