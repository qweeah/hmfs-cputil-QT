#-------------------------------------------------
#
# Project created by QtCreator 2016-05-03T08:02:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cp-utils
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cpdetail_form.cpp \
    blkdetail_form.cpp

HEADERS  += mainwindow.h \
    cpdetail_form.h \
    blkdetail_form.h

FORMS    += mainwindow.ui \
    cpdetail_form.ui \
    blkdetail_form.ui
