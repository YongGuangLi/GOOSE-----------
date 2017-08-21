#-------------------------------------------------
#
# Project created by QtCreator 2015-08-24T15:18:41
#
#-------------------------------------------------

QT       += core xml

QT       -= gui

TARGET = dtxy_scd
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    scd_ied.cpp \
    scd.cpp \
    scd_datatypetemplates.cpp \
    scd_comm.cpp \
    scd_Communication.cpp \
    scd_interface.cpp

HEADERS += \
    scd_ied.h \
    scd.h \
    scd_datatypetemplates.h \
    scd_comm.h \
    scd_Communication.h \
    scd_interface.h
