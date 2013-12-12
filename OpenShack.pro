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

macx:ICON = res/openshack.icns
win32:RC_FILE += res/openshack.rc

unix {
  isEmpty(PREFIX) {
    PREFIX = /usr/local
  }

  target.path = $$PREFIX/bin

  desktop.path = $$PREFIX/share/applications/
  desktop.files += $${TARGET}.desktop

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

SOURCES += main/main.cpp \
    main/mainwindow.cpp \
    widget/newcontactwidget.cpp \
    widget/dxwidget.cpp \
    widget/logbookwidget.cpp \
    widget/mapwidget.cpp \
    interface/rig.cpp \
    interface/fldigi.cpp \
    callbook/hamqth.cpp \
    data/cty.cpp \
    dialog/settingsdialog.cpp \
    dialog/importdialog.cpp \
    logformat/logformat.cpp \
    logformat/adif.cpp \
    main/utils.cpp \
    model/rigtypemodel.cpp

HEADERS += main/mainwindow.h \
    widget/newcontactwidget.h \
    widget/dxwidget.h \
    widget/logbookwidget.h \
    widget/mapwidget.h \
    interface/rig.h \
    interface/fldigi.h \
    callbook/hamqth.h \
    data/cty.h \
    dialog/settingsdialog.h \
    dialog/importdialog.h \
    logformat/logformat.h \
    logformat/adif.h \
    main/utils.h \
    model/rigtypemodel.h

FORMS += main/mainwindow.ui \
    widget/newcontactwidget.ui \
    widget/dxwidget.ui \
    widget/logbookwidget.ui \
    widget/mapwidget.ui \
    dialog/settingsdialog.ui \
    dialog/importdialog.ui

OTHER_FILES += \
    res/stylesheet.css \
    res/openshack.rc \
    res/openshack.desktop

RESOURCES += \
    i18n/i18n.qrc \
    res/res.qrc

TRANSLATIONS = i18n/openshack_de.ts

win32: LIBS += -L$$PWD/../lib/ -lhamlib-2
else:unix: LIBS += -L$/usr/lib/ -lhamlib

win32: INCLUDEPATH += $$PWD/../include/
else:unix: INCLUDEPATH += /usr/include

win32: DEPENDPATH += $$PWD/../include/
else:unix: DEPENDPATH += /usr/include
