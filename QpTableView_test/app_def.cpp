#include "app_def.h"

#include <QProcessEnvironment>
#include <QDebug>
#include <QSettings>
#include <QColor>


const char * const appDef::IN_OS_WIN_PROGRAMMDATA_OUR_PROG_NAME_DIR     = "QpTableView_test";
const char * const appDef::PROGRAM_NAME                                 = "QpTableView test";

const QString appDef::REESTR_ORGANIZATION_NAME           = "BIT Ltd";
const QString appDef::REESTR_APPLICATION_NAME           = "BIT Sale";
const QString appDef::TBL_TEST_NAME                     = "goods";
const QString appDef::DB_TEST_NAME                     = "test_db.db";

const QColor appDef::blue = QColor(67, 101, 255);
const QColor appDef::green = QColor(0, 149, 0);
const QColor appDef::red = QColor(194, 36, 8);
const QColor appDef::brown = QColor(144, 82, 37);


QString appDef::midlight                                     = "";
QString appDef::light                                     = "";
QString appDef::background                                     = "";
QString appDef::highlight                                     = "";
QString appDef::dark                                     = "";
QString appDef::mid                                     = "";
QString appDef::btn_back                                     = "";
QString appDef::em1                                     = "16";
QString appDef::em2                                     = "32";
QString appDef::em3                                     = "48";
QString appDef::em4                                     = "64";


QSettings *appDef::reestr = new QSettings( QSettings::UserScope, appDef::REESTR_ORGANIZATION_NAME, appDef::REESTR_APPLICATION_NAME ) ;


QString appDef::OS_WIN_ProgramData_DIR        = QProcessEnvironment::systemEnvironment().value( "ALLUSERSPROFILE" );


appDef::appDef()
{


}
