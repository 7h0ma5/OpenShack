#-------------------------------------------------
#
# Project created by QtCreator 2013-10-23T11:04:48
#
#-------------------------------------------------

QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = openshack
TEMPLATE = app
VERSION = 0.1.0

DEFINES += VERSION=\\\"$$VERSION\\\"

macx:ICON = res/openshack.icns
win32:RC_FILE += res/openshack.rc

unix {
  isEmpty(PREFIX) {
    PREFIX = /usr/local
  }

  target.path = $$PREFIX/bin

  desktop.path = $$PREFIX/share/applications/
  desktop.files += res/$${TARGET}.desktop

  icon.path = $$PREFIX/share/icons/hicolor/256x256/apps
  icon.files += res/$${TARGET}.png

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

SOURCES += core/main.cpp \
    core/rig.cpp \
    core/fldigi.cpp \
    core/hamqth.cpp \
    core/cty.cpp \
    core/logformat.cpp \
    core/adif.cpp \
    core/utils.cpp \
    core/rigtypemodel.cpp \
    ui/mainwindow.cpp \
    ui/newcontactwidget.cpp \
    ui/dxwidget.cpp \
    ui/logbookwidget.cpp \
    ui/mapwidget.cpp \
    ui/settingsdialog.cpp \
    ui/importdialog.cpp \
    core/migration.cpp \
    ui/dbdialog.cpp \
    ui/exportdialog.cpp

HEADERS += core/rig.h \
    core/fldigi.h \
    core/hamqth.h \
    core/cty.h \
    core/logformat.h \
    core/adif.h \
    core/utils.h \
    core/rigtypemodel.h \
    ui/mainwindow.h \
    ui/newcontactwidget.h \
    ui/dxwidget.h \
    ui/logbookwidget.h \
    ui/mapwidget.h \
    ui/settingsdialog.h \
    ui/importdialog.h \
    core/migration.h \
    ui/dbdialog.h \
    ui/exportdialog.h

FORMS += ui/mainwindow.ui \
    ui/newcontactwidget.ui \
    ui/dxwidget.ui \
    ui/logbookwidget.ui \
    ui/mapwidget.ui \
    ui/settingsdialog.ui \
    ui/importdialog.ui \
    ui/dbdialog.ui \
    ui/exportdialog.ui

OTHER_FILES += \
    res/stylesheet.css \
    res/openshack.rc \
    res/openshack.desktop

RESOURCES += \
    i18n/i18n.qrc \
    res/res.qrc \
    data/data.qrc

TRANSLATIONS = i18n/openshack_de.ts

win32: LIBS += -L$$PWD/../lib/ -lhamlib-2
else:unix: LIBS += -L$/usr/lib/ -lhamlib

win32: INCLUDEPATH += $$PWD/../include/
else:unix: INCLUDEPATH += /usr/include

win32: DEPENDPATH += $$PWD/../include/
else:unix: DEPENDPATH += /usr/include
