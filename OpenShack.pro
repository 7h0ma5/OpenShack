#-------------------------------------------------
#
# Project created by QtCreator 2013-10-23T11:04:48
#
#-------------------------------------------------

QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenShack
TEMPLATE = app
VERSION = 1.0.0

DEFINES += VERSION=\\\"$$VERSION\\\"

macx:ICON = data/icon.icns
win32:RC_FILE += openshack.rc

unix {
  isEmpty(PREFIX) {
    PREFIX = /usr/local
  }

  INSTALLS += target
  target.path = $$PREFIX/bin
}

SOURCES += main.cpp \
    mainwindow.cpp \
    widget/newcontactwidget.cpp \
    widget/dxwidget.cpp \
    widget/logbookwidget.cpp \
    widget/mapwidget.cpp \
    interface/rig.cpp \
    interface/fldigi.cpp \
    callbook/hamqth.cpp \
    db/cty.cpp \
    dialog/settingsdialog.cpp \
    dialog/importdialog.cpp \
    logformat/logformat.cpp \
    logformat/adif.cpp \
    utils.cpp

HEADERS += mainwindow.h \
    widget/newcontactwidget.h \
    widget/dxwidget.h \
    widget/logbookwidget.h \
    widget/mapwidget.h \
    interface/rig.h \
    interface/fldigi.h \
    callbook/hamqth.h \
    db/cty.h \
    dialog/settingsdialog.h \
    dialog/importdialog.h \
    logformat/logformat.h \
    logformat/adif.h \
    utils.h

FORMS += mainwindow.ui \
    widget/newcontactwidget.ui \
    widget/dxwidget.ui \
    widget/logbookwidget.ui \
    widget/mapwidget.ui \
    dialog/settingsdialog.ui \
    dialog/importdialog.ui

OTHER_FILES += \
    stylesheet.css \
    openshack.rc

RESOURCES += \
    i18n.qrc \
    data.qrc

TRANSLATIONS = openshack_de.ts

win32: LIBS += -L$$PWD/../lib/ -lhamlib-2
else:unix: LIBS += -L$/usr/lib/ -lhamlib

win32: INCLUDEPATH += $$PWD/../include/
else:unix: INCLUDEPATH += /usr/include

win32: DEPENDPATH += $$PWD/../include/
else:unix: DEPENDPATH += /usr/include
