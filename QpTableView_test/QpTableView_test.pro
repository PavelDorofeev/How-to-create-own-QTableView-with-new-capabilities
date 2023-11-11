#-------------------------------------------------
#
# Project created by QtCreator 2023-09-07T19:08:09
#
#-------------------------------------------------

QT       += core gui sql

CONFIG += windows

TARGET = QpTableViewTest2
TEMPLATE = app

OBJECTS_DIR     = tmp\\obj\\$$out_dir
MOC_DIR         = tmp\\moc\\$$out_dir
RCC_DIR         = tmp\\rcc\\$$out_dir
UI_DIR          = ui

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES +=MY_DEBUG

include(logging_system/logging_system.pri)
include(out/out.pri)

SOURCES += main.cpp\
    app_def.cpp \
    delegates/combobox_delegate.cpp \
    dialog.cpp \
    section_settings_dlg.cpp

HEADERS  += \
    app_def.h \
    delegates/combobox_delegate.h \
    dialog.h \
    section_settings_dlg.h

FORMS    += \
    dialog.ui \
    section_settings_dlg.ui

message ( PWD = $$PWD)
message ( OUT_PWD = $$OUT_PWD)


PWD_WIN = $${PWD}/db
#CONFIG(release, debug|release): DESTDIR_WIN = $$OUT_PWD/release
#CONFIG(debug, debug|release): DESTDIR_WIN = $$OUT_PWD/debug
DESTDIR_WIN = $$OUT_PWD

PWD_WIN ~= s,/,\\,g
DESTDIR_WIN ~= s,/,\\,g

copyfiles.commands = $$quote(cmd /c xcopy /S /I /Y $$PWD_WIN $$DESTDIR_WIN)
message ( copyfiles.commands = $$copyfiles.commands)

QMAKE_EXTRA_TARGETS += copyfiles

PRE_TARGETDEPS += copyfiles






