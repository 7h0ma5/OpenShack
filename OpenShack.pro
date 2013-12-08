#-------------------------------------------------
#
# Project created by QtCreator 2013-10-23T11:04:48
#
#-------------------------------------------------

QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = openshack
TEMPLATE = app
VERSION = 1.0.0

DEFINES += VERSION=\\\"$$VERSION\\\"

macx:ICON = data/openshack.icns
win32:RC_FILE += openshack.rc

unix {
  isEmpty(PREFIX) {
    PREFIX = /usr/local
  }

  target.path = $$PREFIX/bin

  desktop.path = $$PREFIX/share/applications/
  desktop.files += $${TARGET}.desktop

  icon.path = $$PREFIX/share/icons/hicolor/256x256/apps
  icon.files += data/$${TARGET}.png

  INSTALLS += target desktop icon
}

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all

SOURCES += main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    newcontactwidget.cpp \
    dxwidget.cpp \
    logbookwidget.cpp \
    adif.cpp \
    utils.cpp \
    rig.cpp \
    mapwidget.cpp \
    hamqth.cpp \
    cty.cpp \
    fldigi.cpp

HEADERS += mainwindow.h \
    settingsdialog.h \
    newcontactwidget.h \
    dxwidget.h \
    logbookwidget.h \
    adif.h \
    utils.h \
    rig.h \
    mapwidget.h \
    hamqth.h \
    cty.h \
    fldigi.h

FORMS += mainwindow.ui \
    settingsdialog.ui \
    newcontactwidget.ui \
    dxwidget.ui \
    logbookwidget.ui \
    mapwidget.ui

OTHER_FILES += \
    stylesheet.css \
    openshack.rc \
    openshack.desktop

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
