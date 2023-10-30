#-------------------------------------------------
#
# Project created by QtCreator 2023-09-07T19:08:09
#
#-------------------------------------------------

QT       += core gui sql

CONFIG += windows

TARGET = QpTableViewTest
TEMPLATE = app

OBJECTS_DIR     = tmp\\obj\\$$out_dir
MOC_DIR         = tmp\\moc\\$$out_dir
RCC_DIR         = tmp\\rcc\\$$out_dir
UI_DIR          = ui


#INCLUDEPATH += D:\\QtSDK1.2.1\\QtSources\\4.8.1\\src\\gui\\itemviews
#DEPENDPATH  += D:\\QtSDK1.2.1\\QtSources\\4.8.1\\src\\gui\\itemviews

INCLUDEPATH += ../../my_lib
DEPENDPATH += ../../my_lib


INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
INCLUDEPATH += ../../
DEPENDPATH += ../../
INCLUDEPATH += views
DEPENDPATH += views

DEFINES +=MY_DEBUG

include(logging_system/logging_system.pri)

#include(views/views.pri)

SOURCES += main.cpp\
    app_def.cpp \
    delegates/combobox_delegate.cpp \
    dialog.cpp

HEADERS  += \
    app_def.h \
    delegates/combobox_delegate.h \
    dialog.h

FORMS    += \
    dialog.ui

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






