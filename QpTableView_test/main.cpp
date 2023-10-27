#include <QtGui/QApplication>
#include "dialog.h"
#include "app_def.h"
#include <QDebug>

#include "logging_system/logging_system.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // -----------------------------------------------------------------
    //                      logging_System
    // -----------------------------------------------------------------


    logging_System::LOG_TYPES logTypes = (logging_System::LOG_WARNING
                                          |logging_System::LOG_CRITICAL
                                          |logging_System::LOG_FATAL);

    bool logging_history_ON = true;

#if defined(MY_DEBUG)

    logTypes |= logging_System::LOG_DEBUG; // не подсвечивается , но работает

    logging_history_ON = false;

#endif

    qDebug() << appDef::OS_WIN_ProgramData_DIR+"\\"+appDef::IN_OS_WIN_PROGRAMMDATA_OUR_PROG_NAME_DIR+"\\"+logging_System::LOGS_DIR;


    bool ll = logging_System::init(

                logging_System::LOGS_DIR,

                logging_System::LOG_FILE_NAME,
                logTypes,
                false,
                logging_history_ON );

    if( ! ll)
        return 0;

    qInstallMsgHandler(logging_System::logCatcher);


    QString st = "*{\n"\
            "   font-size:30px;\n"\
            "   font-weight:500;\n"\
            "   color:gray;\n"\
            "}" \
            "QPushButton{\n"\
            "   font-size:40px;\n"\
            "   color:#274a30;\n"\
            "}" \
            "Dialog{\n"\
            "   padding:200px;\n"\
            "   margin:200px;\n"\
            "}" \
            "QpHorHeaderView:section{\n"\
            "   background:#b2d1f8;\n"\
            "}" \
            "QpTableView:item{\n"\
            "}" \
            "QpHorHeaderView:section{\n"\
            "   background:#b2d1f8;\n"\
            "}" \
            "QpVertHeaderView:section{\n"\
            "   background:#fca842;\n"\
            "}" \

            "QPlainTextEdit{\n"\
            "   width:200px;\n"\
            "   color:#38532a;\n"\
            "   padding:40px;\n"\
            "   font-weight:900;\n"\
            "}" \
            ;

    app.setStyleSheet( st );

    qDebug() << app.styleSheet();



    Dialog w;

    Qt::WindowFlags flags = w.windowFlags();

    flags |= Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;

    w.setWindowFlags( flags );

    w.showNormal();
    
    return app.exec();
}
