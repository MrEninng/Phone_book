#-------------------------------------------------
#
# Project created by QtCreator 2015-10-25T19:48:51
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = phone_book
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    changeinfobutton_window.cpp \
    gender_diagram.cpp

HEADERS  += mainwindow.h \
    changeinfobutton_window.h \
    gender_diagram.h

FORMS    += mainwindow.ui \
    changeinfobutton_window.ui \
    gender_diagram.ui

RESOURCES += \
    resourses.qrc
CONFIG += c++11
