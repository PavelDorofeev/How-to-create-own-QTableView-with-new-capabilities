#include "app_def.h"

#include <QProcessEnvironment>
#include <QDebug>
#include <QSettings>
#include "db/reestr.h"



const char * const appDef::IN_OS_WIN_PROGRAMMDATA_OUR_PROG_NAME_DIR     = "QTableView_Cells";
const char * const appDef::PROGRAM_NAME                                 = "QTableView Cells";

const QString appDef::REESTR_ORGANIZATION_NAME           = "BIT Ltd";
const QString appDef::REESTR_APPLICATION_NAME           = "BIT Sale";

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


Reestr *appDef::reestr = new Reestr( QSettings::UserScope, appDef::REESTR_ORGANIZATION_NAME, appDef::REESTR_APPLICATION_NAME ) ;



QString appDef::OS_WIN_ProgramData_DIR        = QProcessEnvironment::systemEnvironment().value( "ALLUSERSPROFILE" );


appDef::appDef()
{


}
