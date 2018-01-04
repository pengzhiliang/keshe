#-------------------------------------------------
#
# Project created by QtCreator 2017-10-29T16:42:03
#
#-------------------------------------------------

QT       += core gui webkit
QT       += network
QT       += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pro2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gprmc.cpp \
    china_shift.cpp \
    dialog.cpp

HEADERS  += mainwindow.h \
    gprmc.h \
    china_shift.h \
    dialog.h \
    my_struct.h

FORMS    += mainwindow.ui \
    dialog.ui

RESOURCES += \
    icon.qrc

