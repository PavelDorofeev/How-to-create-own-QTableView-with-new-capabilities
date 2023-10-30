#include "dialog.h"
#include "ui_dialog.h"

#include <QDebug>
#include <QComboBox>
#include <QScrollBar>
#include <QMessageBox>
#include <QSqlError>
#include <QDir>
#include "delegates/combobox_delegate.h"
#include "app_def.h"


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    mdl_sql(0),
    mdl_standart(0)
{
    ui->setupUi(this);

    setWindowTitle( QString::fromUtf8("QpTableView с шаблоном расположения секций"));

    init_StandardItemModel();
}

Dialog::~Dialog()
{
    delete ui;
}


bool Dialog::init_sql_db()
{
    QString dbDir = appDef::OS_WIN_ProgramData_DIR+"/"+appDef::IN_OS_WIN_PROGRAMMDATA_OUR_PROG_NAME_DIR;


    qDebug() << "init_sql ";


    QString full_file_name = QDir::currentPath()+"/"+appDef::DB_TEST_NAME ;

    qDebug() << " full_file_name " << full_file_name << QFile( full_file_name ).exists();

    db = QSqlDatabase::addDatabase("QSQLITE");

    if(db.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning( this,
                              QString::fromUtf8("Ошибка открытия базы данных"),
                              db.lastError().text()
                              );
        return false;

    }


    if( ! QFile( full_file_name ).exists())
    {
        QMessageBox::warning( this,
                              QString::fromUtf8("Внимание"),
                              QString::fromUtf8("файл базы данных не найден : %1")
                              .arg(full_file_name)
                              );

        return false;

    }

    db.setDatabaseName( full_file_name );

    if ( ! db.open( ) )
    {
        qCritical() << QString::fromUtf8("Невозможно открыть файл с базой данных ") << full_file_name;

        QMessageBox::warning( this,
                              QString::fromUtf8("Сбой программы"),
                              QString::fromUtf8("Не удается открыть базу данных\n%1")
                              .arg(db.lastError().text())
                              );

        return false;
    }

    return true;

}
bool Dialog::init_sql_model()
{
    if( mdl_sql !=0 )
        delete mdl_sql;

    mdl_sql = new QSqlTableModel( this , db);

    Qp_SECTION_TMPL matrix = prepare_matrix( *ui->txt3);

    mdl_sql->setTable( appDef::TBL_TEST_NAME);

    if ( !mdl_sql->select())
    {
        QMessageBox::warning( this,
                              QString::fromUtf8("Ошибка"),
                              QString::fromUtf8("Не удается открыть таблицу %1")
                              .arg(appDef::TBL_TEST_NAME)
                              .arg(db.lastError().text())
                              );

        return false;
    }

    ui->tableView->setModel( mdl_sql , matrix );



    ui->tableView->setShowGrid( true );

    qDebug() << " model2->rowCount() : " << mdl_sql->rowCount();
    qDebug() << " model2->columnCount() : " << mdl_sql->columnCount();



    return true;
}

void Dialog::init_StandardItemModel()
{
    if( mdl_standart !=0 )
        delete mdl_standart;

    mdl_standart = new QStandardItemModel( this);

    mdl_standart->setHorizontalHeaderItem(0, new QStandardItem(QString("0000")));
    mdl_standart->setHorizontalHeaderItem(1, new QStandardItem(QString("1111")));
    mdl_standart->setHorizontalHeaderItem(2, new QStandardItem(QString("2222")));
    mdl_standart->setHorizontalHeaderItem(3, new QStandardItem(QString("3333")));
    mdl_standart->setHorizontalHeaderItem(4, new QStandardItem(QString("4444")));
    mdl_standart->setHorizontalHeaderItem(5, new QStandardItem(QString("5555")));
    //model->setHorizontalHeaderItem(6, new QStandardItem(QString("6666")));
    //    model->setHorizontalHeaderItem(7, new QStandardItem(QString("7777")));

    int colCount = mdl_standart->columnCount();

    int row = 0;

    int rowCount = 5;

    while ( row < rowCount )
    {
        bool bbb = mdl_standart->insertRow( row );

        int fldNumber = 0;

        while ( fldNumber < colCount )
        {
            if(fldNumber == 4 )
            {
                QStandardItem *item = new QStandardItem( QString::fromUtf8("длинная строка для примера") );

                mdl_standart->setItem(row, fldNumber, item );

            }
            else if(fldNumber == 3 )
            {
                QStandardItem *item = new QStandardItem( "1" );

                mdl_standart->setItem(row, fldNumber, item );

            }
            else
            {
                QStandardItem *item = new QStandardItem( QString("r:%1  c:%2")
                                                         .arg( row )
                                                         .arg( fldNumber )
                                                         );
                mdl_standart->setItem(row, fldNumber, item);
            }

            fldNumber++;
            //col++;
        }

        row++;
    }

    for( int row=0; row< mdl_standart->rowCount(); row++)
    {
        qDebug() << "row :" << row ;

        for( int col=0; col< mdl_standart->columnCount(); col++)
        {
            qDebug() << "   col :" << col << " data" << mdl_standart->data( mdl_standart->index( row, col ));
        }
    }

    QStringList lst;

    lst << "aaaa"<<"bbbbb"<<"ccccc";

    ComboBoxDelegate *cmbd = new ComboBoxDelegate( lst , this);

    ui->tableView->setItemDelegateForColumn( 3 , cmbd );


    //    QList< QList<int> > cells_tmpl;

    //    cells_tmpl.append(  QList<int>() << 0 << 1 << 2 << 2 << 5  );
    //    cells_tmpl.append(  QList<int>() << 0 << 1 << 3 << 4 << 5  );
    //lst.append(  QList<int>() << 3 << 3 << 3 << 4 << 5  );

    Qp_SECTION_TMPL matrix = prepare_matrix( *ui->txt3);

    ui->tableView->setModel( mdl_standart , matrix );

    //on_btn_init_sections_3_clicked();

    //    ui->tableView->horizontalHeader()->init( cells_tmpl );

    //    ui->tableView->setModel( model );

    ui->tableView->setShowGrid( true );

    qDebug() << " model->rowCount() : " << mdl_standart->rowCount();
    qDebug() << " model->columnCount() : " << mdl_standart->columnCount();
    //    qDebug() << " ui->tableView->selectionBehavior : " << ui->tableView->selectionBehavior();

    ui->tableView->setMouseTracking( false );
}


void Dialog::on_btn_Update_clicked()
{
    update();
}




void Dialog::on_btn_init_sections_2_clicked()
{
    Qp_SECTION_TMPL matrix =  prepare_matrix ( *ui->txt2);
    ui->tableView->init_template( matrix );

}

void Dialog::on_btn_init_sections_clicked()
{
    Qp_SECTION_TMPL matrix =  prepare_matrix ( *ui->txt1);
    ui->tableView->init_template( matrix );

}

void Dialog::on_btn_init_sections_3_clicked()
{
    Qp_SECTION_TMPL matrix =  prepare_matrix ( *ui->txt3);
    ui->tableView->init_template( matrix );

}


Qp_SECTION_TMPL Dialog::prepare_matrix( const QPlainTextEdit & txt )
{
    QStringList lst = txt.toPlainText().split("\n");

    qDebug() << "prepare_matrix :\n" << txt.document()->toPlainText();

    Qp_SECTION_TMPL matrix;

    foreach ( QString str, lst)
    {
        QStringList lst1 = str.split(",");

        QList < int > lst;

        foreach( QString nn, lst1)
        {
            lst.append( nn.toInt() );
        }

        matrix.append( lst );
    }

    return matrix;

}

void Dialog::on_btn_sqltableModel_On_clicked()
{
    init_sql_db();

    init_sql_model();
}

void Dialog::on_btn_QStandardItemModel_On_clicked()
{
    init_StandardItemModel();
}
