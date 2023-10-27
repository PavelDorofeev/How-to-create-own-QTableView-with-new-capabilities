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


INCLUDEPATH += D:\\QtSDK1.2.1\\QtSources\\4.8.1\\src\\gui\\itemviews
DEPENDPATH  += D:\\QtSDK1.2.1\\QtSources\\4.8.1\\src\\gui\\itemviews

INCLUDEPATH += ../../my_lib
DEPENDPATH += ../../my_lib


INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
INCLUDEPATH += ../../
DEPENDPATH += ../../
INCLUDEPATH += views
DEPENDPATH += views

DEFINES +=MY_DEBUG

include(../../my_lib/logging_system/logging_system.pri)

#include(views/views.pri)

SOURCES += main.cpp\
    app_def.cpp \
    ../../db/reestr.cpp \
    ../../my_lib/pbltableview/combobox_delegate.cpp \
    dialog.cpp

HEADERS  += \
    app_def.h \
    ../../db/reestr.h \
    ../../my_lib/pbltableview/combobox_delegate.h \
    dialog.h

FORMS    += \
    dialog.ui

#message ( COPY_DIR = $(COPY_DIR))
message ( PWD = $$PWD)
message ( OUT_PWD = $$OUT_PWD)

#copydata.commands = $(COPY_DIR) $$shell_path($$PWD/db)) $$OUT_PWD
#first.depends = $(first) copydata
#export(first.depends)
#export(copydata.commands)
#QMAKE_EXTRA_TARGETS += first copydata

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






