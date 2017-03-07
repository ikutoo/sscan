#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T15:49:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mine_scanner
TEMPLATE = app


SOURCES += main.cpp\
        cmainwin.cpp \
    scan/scan_alive.c \
    scan/scan_core.c \
    scan/scan_port.c \
    scan/scan_sysInfo.c \
    scan/send_pkt.c \
    scan/utilities.c

HEADERS  += cmainwin.h \
    scan/define.h \
    scan/scan_alive.h \
    scan/scan_core.h \
    scan/scan_port.h \
    scan/scan_sysInfo.h \
    scan/send_pkt.h \
    scan/utilities.h

FORMS    += cmainwin.ui
