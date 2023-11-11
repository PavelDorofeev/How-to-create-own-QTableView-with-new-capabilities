#ifndef APP_DEF_H
#define APP_DEF_H

#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>
#include <QSettings>

//#include "equipment/abstract_kkt/a_kkt.h"

class QSettings;

class appDef
{

public:


    static QString        midlight;
    static QString        light;
    static QString        background;
    static QString        highlight;
    static QString        dark;
    static QString        mid;
    static QString        btn_back;

    static QSettings         *reestr;

    static QString        em1;
    static QString        em2;
    static QString        em3;
    static QString        em4;

    static QString        OS_WIN_ProgramData_DIR;

    static const QString        tab_selected_background;

    static const QString REESTR_ORGANIZATION_NAME;
    static const QString REESTR_APPLICATION_NAME;
    static const QString TBL_TEST_NAME;
    static const QString DB_TEST_NAME;

    static const char * const IN_OS_WIN_PROGRAMMDATA_OUR_PROG_NAME_DIR;
    static const char * const PROGRAM_NAME;
    static const QString const PROGRAM_NAME_RU;




public:

    appDef();

};

#endif // APP_DEF_H
