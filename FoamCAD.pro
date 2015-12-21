#-------------------------------------------------
#
# Project created by QtCreator 2015-12-19T18:07:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FoamCAD
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        MainWindow.cpp \
    CObject.cpp \
    CPoint.cpp \
    CLine.cpp \
    CadEditForm.cpp

HEADERS  += MainWindow.h \
    CObject.h \
    CPoint.h \
    Relative.h \
    Point.h \
    CLine.h \
    CadEditForm.h

FORMS    += MainWindow.ui \
    CadEditForm.ui

RESOURCES += \
    images.qrc
