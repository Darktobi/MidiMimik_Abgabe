QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = MidiMimik_v2

INCLUDEPATH += C:\Users\Nick\OpenCV3.1\opencv\build\include

LIBS +=     C:\Users\Nick\OpenCV3.1\opencv\build\x64\vc14\lib\opencv_world310.lib \
            C:\Users\Nick\OpenCV3.1\opencv\build\x64\vc14\lib\opencv_world310d.lib \

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp

include(../drumstick/drumstick.pro)

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui



