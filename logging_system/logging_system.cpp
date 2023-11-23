#include "logging_system.h"

#include <QDesktopServices>

#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <io.h>
#include <stdio.h>
#include <iostream>



#include <QUrl>

/* Необходима инициализация статических переменных */

logging_System * logging_System::logg = NULL;

QString logging_System::logFilePath     = QString();
QString logging_System::dirName         = QString();
QString logging_System::fileName        = QString();

QFile *logging_System::logFile = 0;

#include <stdio.h>

const QString logging_System::LOGS_DIR              = "log";
const QString logging_System::LOGS_HYSTORY_DIR      = "logs";
const QString logging_System::LOG_FILE_NAME         = "log.txt";

bool logging_System::debug_Enabled =false;
bool logging_System::warning_Enabled =false;
bool logging_System::critical_Enabled =false;
bool logging_System::fatal_Enabled =false;

bool logging_System::toStdOut_ = true;
bool logging_System::history_ON = true;

logging_System::LOG_TYPES logging_System::types = 0;


logging_System::logging_System()//QObject *parent) //:    QObject(parent)
{


}


bool logging_System::init(QString dirName_,
                          QString fileName_,
                          LOG_TYPES debug_types,
                          bool stdOut_,
                          bool history_ON_)
{
    history_ON = history_ON_;

    types = debug_types;

    if(types & LOG_DEBUG)
        debug_Enabled = true;

    if(types & LOG_WARNING)
        warning_Enabled = true;

    /* if(types & LOG_CRITICAL)
        critical_Enabled = true;

    if(types & LOG_FATAL)
        fatal_Enabled = true;*/

    dirName=dirName_;
    fileName=fileName_;

    qDebug() << " logging : "<< dirName << fileName;

    if( ! stdOut_)
    {
        toStdOut_ = stdOut_;

        if(! QDir(dirName).exists())
        {
            if( ! QDir().mkdir(dirName))
            {
                qCritical()  << "can't create folder  (for log file) :  " << dirName;

                QMessageBox::critical( 0,
                          QString::fromUtf8("Ошибка"),
                          QString::fromUtf8("Не удалось создать каталог для логирования программы :\n\n %1").
                          arg(dirName),
                          QString::fromUtf8("Возможно не хватает прав администратора.")
                          );

                return false;
            }

            qWarning()  << "create folder (for log file) :  " << dirName;
        }

        if(! QDir(dirName+"\\"+LOGS_HYSTORY_DIR).exists())
        {
            if( ! QDir().mkdir(dirName +"\\"+LOGS_HYSTORY_DIR))
            {
                qCritical()  << "can't create folder  (for log file) :  " << dirName+"\\"+LOGS_HYSTORY_DIR;

                QMessageBox::critical( 0,
                          QString::fromUtf8("Ошибка"),
                          QString::fromUtf8("Не удалось создать каталог для хранения логов работы программы :\n\n %1").
                          arg(dirName),
                          QString::fromUtf8("Возможно не хватает прав администратора.")
                          );

                return false;
            }
            qWarning()  << "create folder  (for old log files) :  " << dirName+"\\"+LOGS_HYSTORY_DIR;
        }

        logFilePath = dirName + "\\"+fileName;

        qDebug() << " logFilePath : " << logFilePath;

        logFile  = new QFile(logFilePath);


        if( ! testLargeFileAndSave(logFilePath) )
            ;

        if( ! logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
        {
            QMessageBox::critical(0,
                     QString::fromUtf8("Сбой запуска программы"),
                     QString::fromUtf8("К сожалению не удается открыть лог файл  : \n%1").
                     arg(logFilePath));

            qCritical()  << "can't open log file :  " << logFilePath;

            return false;
        }

        FILE *loggingStreamStdout;
        FILE *loggingStreamStderr;

        int fd = logFile->handle();

        freopen_s ( &loggingStreamStdout, "\\\\.\\NUL", "w", stdout);
        setvbuf ( loggingStreamStdout, NULL, _IONBF, 0);

        freopen_s (&loggingStreamStderr, "\\\\.\\NUL", "w", stderr);
        setvbuf ( loggingStreamStderr, NULL, _IONBF, 0);

        _dup2 (fd, _fileno(stdout));
        _dup2 (fd, _fileno(stderr));

        //freopen(m_logFile_.fileName().toLocal8Bit().data(), "w", stdout); // в начало stdout потом qDebug()
    }

    /*
*/
    /*   old_stdout = _dup( 1 );   // "old" now refers to "stdout"
    // Note:  file descriptor 1 == "stdout"
    if( old_stdout == -1 )
    {
        perror( "_dup( 1 ) failure" );
        exit( 1 );
    }

    _write( old_stdout, "This goes to stdout first\n", 26 );

    if( fopen_s( &stdOutFile, "data", "w" ) != 0 )
    {
        puts( "Can't open file 'data'\n" );
        exit( 1 );
    }

    stream = new QTextStream(stdOutFile ,QIODevice::WriteOnly|QIODevice::ReadOnly);

    //stream << QString("sdfasfadsfadsfadsfads");

    // stdout now refers to file "data"
    if( -1 == _dup2( _fileno( stdOutFile ), 1 ) )
    {
        perror( "Can't _dup2 stdout" );
        exit( 1 );
    }
    puts( "This goes to file 'data'\n" );

    // Flush stdout stream buffer so it goes to correct file
    fflush( stdout );
    fclose( stdOutFile );

    // Restore original stdout
    _dup2( old_stdout, 1 );
    puts( "This goes to stdout\n" );
    puts( "The file 'data' contains:" );
    _flushall();
    system( "type data" );*/

    return true;
}

logging_System::~logging_System()
{
    if( logg != NULL)
        logg = NULL;

    if(logFile != 0)
        delete logFile;
    logFile =0;
}

void logging_System::logCatcher(QtMsgType type, const char *msg)
{

    //if( logging_System::logg != NULL)

        logging_System::mess(type , msg);

}

void logging_System::mess ( QtMsgType type,const QString & msg)
{

    QString format="hh:mm:ss zzz";

    if(logFile ==0)
        return;

    switch (type)
    {
    case QtDebugMsg:

        if( debug_Enabled )
        {
            if(toStdOut_)
            {
                std::cout << QDateTime::currentDateTime().toString(format).toLocal8Bit().constData() << std::flush;
                std::cout << QString(" Debug: ").toUtf8().constData() << std::flush;
                std::cout << QString(msg+'\n').toUtf8().constData() << std::flush;
            }
            else
            {
                logFile->write(QDateTime::currentDateTime().toString(format).toUtf8());
                if(msg != "\n" && msg != "\n\n")
                    logFile->write(QString(" Debug: ").toUtf8());

                logFile->write( QString(msg).toUtf8() +"\n");
                logFile->flush();
            }
        }

        break;

    case QtWarningMsg:

        if( warning_Enabled )
        {
            if(toStdOut_)
            {
                std::cout <<QDateTime::currentDateTime().toString("hh:mm:ss ").toUtf8().constData() << std::flush;
                std::cout << QString("Warning: "+ QString(msg)+"\n").toUtf8().constData() << std::flush;
            }
            else
            {
                logFile->write(QDateTime::currentDateTime().toString("hh:mm:ss ").toUtf8());
                logFile->write("Warning: "+ QString(msg).toUtf8()+"\n");
                logFile->flush();
            }
        }
        break;

    case QtCriticalMsg: // = QtSystemMsg

        if(critical_Enabled)
        {
            if(toStdOut_)
            {
                std::cout <<QDateTime::currentDateTime().toString("hh:mm:ss ").toUtf8().constData() << std::flush;
                std::cout << QString("\n---------------- Critical: --------------------\n").toUtf8().constData() << std::flush;
                std::cout << QString(msg).toUtf8().constData() << std::flush;
                std::cout << QString("\n---------------- Critical: --------------------\n").toUtf8().constData()<< std::flush;
            }
            else
            {
                logFile->write(QDateTime::currentDateTime().toString("hh:mm:ss ").toUtf8());
                logFile->write(QString("\n---------------- Critical: --------------------\n").toUtf8());
                logFile->write("Critical: "+ QString(msg).toUtf8());
                logFile->write(QString("\n---------------- Critical: --------------------\n").toUtf8());
                logFile->flush();
            }
        }
        break;

    case QtFatalMsg:

        if(fatal_Enabled)
        {
            if(toStdOut_)
            {
                std::cout << QString("---------------- Critical: --------------------\n").toUtf8().constData()<< std::flush;
                std::cout << QString("Fatal: "+ msg).toUtf8().constData();
                std::cout << QString("---------------- Critical: --------------------\n").toUtf8().constData()<< std::flush;
            }
            else
            {

                logFile->write(QString("---------------- Critical: --------------------\n").toUtf8());
                logFile->write("Fatal: "+ QString(msg).toUtf8());
                logFile->write(QString("---------------- Critical: --------------------\n").toUtf8());
                logFile->flush();
            }
        }

        abort();
        break;
    }
}


void logging_System::stdOutDataAvailable(int ii)
{
    qDebug() << "stdOutDataAvailable : " << ii;

}


/*bool logging_System::init()
{
    if ( ! initLoggingFiles())
        return false;

    if ( ! initLoggingSystem())
        return false;



    return true;
}*/

bool logging_System::initLoggingFiles()
{
    /*if(! logFile->isOpen())
        return false;*/

    return true;
}

bool logging_System::initLoggingSystem()
{
    return true;
}

bool logging_System::testLargeFileAndSave(QString &logFilePath)
{
    bool res = false;

    QFile file(logFilePath);

    if( ! file.exists())
        return true;


    if( ! history_ON)
    {
        if( ! file.open(QFile::WriteOnly )) // QFile::Text |QFile::Text |QFile::Truncate))
        {
            return false;
        }

        file.resize(0); // clear !!!
        file.flush();
        file.close();

        return true;
    }

    qint64 sz = file.size();

    if( sz > 100000)
    {
        // QDir toDir(dirName);

        QString fn = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss")+".txt";

        //qDebug() << " dirName " << dirName<< " LOG_FILE_NAME : " << LOG_FILE_NAME << " LOGS_HYSTORY_DIR "<<LOGS_HYSTORY_DIR;

        QString to = dirName+"\\"+ LOGS_HYSTORY_DIR+"\\log-" + fn;
        qDebug() << " to : " << to;
        res = file.rename(to);

        return res;
    }
    else
        return true;
}

bool logging_System::openLoggingOnToOnNotepad()
{
/*    if(logg == NULL)
        return false;*/

    if ( ! QDesktopServices::openUrl( QUrl::fromLocalFile(logFilePath)))
    {
        qCritical() << "ERROR OPEN  logFilePath : " << logFilePath;
        return false;
    }


    return true;
}
