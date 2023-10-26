#include "dialog.h"
#include "ui_dialog.h"

#include <QDebug>
#include <QComboBox>
#include <QScrollBar>
#include "pbltableview/combobox_delegate.h"


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    setWindowTitle( QString::fromUtf8("QpTableView с шаблоном расположения секций"));

    init();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::init()
{

    model = new QStandardItemModel( this);

    model->setHorizontalHeaderItem(0, new QStandardItem(QString("0000")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("1111")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("2222")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("3333")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString("4444")));
    model->setHorizontalHeaderItem(5, new QStandardItem(QString("5555")));
    //    model->setHorizontalHeaderItem(6, new QStandardItem(QString("6666")));
    //    model->setHorizontalHeaderItem(7, new QStandardItem(QString("7777")));

    int colCount = model->columnCount();

    int row = 0;

    int rowCount = 5;

    while ( row < rowCount )
    {
        bool bbb = model->insertRow( row );

        int fldNumber = 0;

        while ( fldNumber < colCount )
        {
            if(fldNumber == 4 )
            {
                QStandardItem *item = new QStandardItem( QString::fromUtf8("длинная строка для примера") );

                model->setItem(row, fldNumber, item );

            }
            else if(fldNumber == 3 )
            {
                QStandardItem *item = new QStandardItem( "1" );

                model->setItem(row, fldNumber, item );

            }
            else
            {
                QStandardItem *item = new QStandardItem( QString("r:%1  c:%2")
                                                         .arg( row )
                                                         .arg( fldNumber )
                                                         );
                model->setItem(row, fldNumber, item);
            }

            fldNumber++;
            //col++;
        }

        row++;
    }

    for( int row=0; row< model->rowCount(); row++)
    {
        qDebug() << "row :" << row ;

        for( int col=0; col< model->columnCount(); col++)
        {
            qDebug() << "   col :" << col << " data" << model->data( model->index( row, col ));
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

    ui->tableView->setModel( model , matrix );

    //on_btn_init_sections_3_clicked();

    //    ui->tableView->horizontalHeader()->init( cells_tmpl );

    //    ui->tableView->setModel( model );

    ui->tableView->setShowGrid( true );

    qDebug() << " model->rowCount() : " << model->rowCount();
    qDebug() << " model->columnCount() : " << model->columnCount();
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
