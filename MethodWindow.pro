#-------------------------------------------------
#
# Project created by QtCreator 2014-12-16T23:56:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets serialport
    QT += widgets
    QT += testlib
    QT += network
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}


TARGET = MethodWindow
TEMPLATE = app
#target.path=/home/pi/Method_Window
#INSTALLS += target


SOURCES += main.cpp\
    methodwindow.cpp \
    globals.cpp \
    methodrun.cpp \
    methodtablemodel.cpp \
    serialhandler.cpp \
    settingsdialog.cpp \
    spinboxdelegate.cpp

HEADERS  += methodwindow.h \
    globals.h \
    methodrun.h \
    methodtablemodel.h \
    serialhandler.h \
    settingsdialog.h \
    spinboxdelegate.h

FORMS    += methodwindow.ui \
    settingsdialog.ui
