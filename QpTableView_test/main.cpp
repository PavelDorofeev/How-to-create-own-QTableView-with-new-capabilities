#include <QtGui/QApplication>
#include "dialog.h"
#include "app_def.h"
#include <QDebug>

#include "logging_system/logging_system.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << " app font " << app.font();
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

    int fontSize = 20; // px

    //             "   font-size:"+QString::number(fontSize)+"px;\n"\

    QFont fnt;
    fnt.setPixelSize( 25);

    app.setFont(fnt);

    QString st = "*{\n"\
            "   font-weight:500;\n"\
            "   color:gray;\n"\
            "}" \

            "QPushButton{\n"\
            "   background:#1a3167;\n"\
            "   color:white;\n"\
            "   padding:"+QString::number(fontSize*0.5)+"px;\n"\
            "   margin:"+QString::number(fontSize*0.5)+"px;\n"\
            "   border-radius:"+QString::number(fontSize*0.3)+"px;\n"\
            "   border: 2px solid gray;\n"\
            "   outline:none;\n"\
            "}" \

            "QPushButton:focus{\n"\
            "   background:#925b26;\n"\
            "   border: 3px solid gray;\n"\
            "}" \

            "QPushButton:hover{\n"\
            "   background:#547f86;\n"\
            "   border: 0px solid gray;\n"\
            "}" \

            "QRadioButton{\n"\
            "   font-size:"+QString::number(fontSize*1.8)+"px;\n"\
            "   margin:"+QString::number(fontSize*0.5)+"px;\n"\
            "}" \

            "QComboBox{\n"\
            "   font-size:"+QString::number(fontSize*1.3)+"px;\n"\
            "   background:#f7efcc;\n"\
            "   color:#454a5f;\n"\
            "   padding:"+QString::number(fontSize*0.5)+"px;\n"\
            "   margin:"+QString::number(fontSize*0.5)+"px;\n"\
            "   border-radius:"+QString::number(fontSize*0.3)+"px;\n"\
            "   border: 2px solid gray;\n"\
            "   outline:none;\n"\
            "}" \

            "QCheckBox::indicator\n"\
            "{\n"\
            "   width: "+QString::number(fontSize)+"px;\n"\
            "   height: "+QString::number(fontSize)+"px;\n"\
            "}\n"\

            "QCheckBox{\n"\
            "   font-size:"+QString::number(fontSize*1.3)+"px;\n"\
            "   background:#5d7a51;\n"\
            "   color:#e2d1e9;\n"\
            "   padding:"+QString::number(fontSize*0.5)+"px;\n"\
            "   margin:"+QString::number(fontSize*0.5)+"px;\n"\
            "   border-radius:"+QString::number(fontSize*0.3)+"px;\n"\
            "   border: 2px solid gray;\n"\
            "}" \

            "QCheckBox::indicator\n"\
            "{\n"\
            "   width: "+QString::number(fontSize)+"px;\n"\
            "   height: "+QString::number(fontSize)+"px;\n"\
            "}\n"\

            "QGroupBox{\n"\
            "   background-color: rgb(245,245,245);\n"\
            "   border: 1px solid gray;\n"\
            "   border-radius: "+QString::number(fontSize*0.5)+"px;\n"\
            "   padding: "+QString::number(fontSize*0.5)+"px;\n"\
            "   margin: "+QString::number(fontSize*0.3)+"px;\n"\
            "}\n"\

            "Dialog{\n"\
            "}" \

            "QpHorHeaderView:section{\n"\
            "   background:#b2d1f8;\n"\
            "}" \

            "QpTableView{\n"\
            "   outline: none;\n"\
            "}" \

            "QpTableView:item:first{\n"\
            "   color:red;\n"\
            "}" \

            "QpTableView:item:first{\n"\
            "   border: 1px dotted red;\n"\
            "}" \

            "QpHorHeaderView:section{\n"\
            "   background:#b2d1f8;\n"\
            "}" \

            "QpVertHeaderView:section{\n"\
            "   background:#fca842;\n"\
            "}" \

            "QPlainTextEdit{\n"\
            "   width:300px;\n"\
            "   color:#57754b;\n"\
            "   padding: "+QString::number(fontSize*0.5)+"px;\n"\
            "   font-weight:900;\n"\
            "}" \
            ;

    app.setStyleSheet( st );

    qDebug() << app.styleSheet();



    Dialog w;

    Qt::WindowFlags flags = w.windowFlags();

    flags |= Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;

    w.setWindowFlags( flags );

    w.showFullScreen();
    
    return app.exec();
}
