#-------------------------------------------------
#
# Project created by QtCreator 2013-10-23T11:04:48
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenShack
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    settingsdialog.cpp \
    newcontactwidget.cpp \
    dxwidget.cpp \
    logbookwidget.cpp \
    adif.cpp \
    utils.cpp \
    rig.cpp \
    mapwidget.cpp \
    rigwidget.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    newcontactwidget.h \
    dxwidget.h \
    logbookwidget.h \
    adif.h \
    utils.h \
    rig.h \
    mapwidget.h \
    rigwidget.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    newcontactwidget.ui \
    dxwidget.ui \
    logbookwidget.ui \
    mapwidget.ui \
    rigwidget.ui

OTHER_FILES +=

RESOURCES += \
    i18n.qrc \
    media.qrc

TRANSLATIONS = openshack_de.ts

win32: LIBS += -L$$PWD/../lib/ -lhamlib-2
else:unix: LIBS += -L$/usr/lib/ -lhamlib

win32: INCLUDEPATH += $$PWD/../include/
else:unix: INCLUDEPATH += /usr/include

win32: DEPENDPATH += $$PWD/../include/
else:unix: DEPENDPATH += /usr/include
