#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QtGui/qp_horheaderview.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    
    void init_StandardItemModel();

    bool init_sql_db();

    bool init_sql_model();

    QSqlDatabase db;

private slots:

    void on_btn_Update_clicked();

    void on_btn_init_sections_2_clicked();

    void on_btn_init_sections_clicked();

    void on_btn_init_sections_3_clicked();

    void on_btn_sqltableModel_On_clicked();

    void on_btn_QStandardItemModel_On_clicked();

private:
    Ui::Dialog *ui;


    Qp_SECTION_TMPL prepare_matrix( const QPlainTextEdit & txt );

    QStandardItemModel * mdl_standart;
    QSqlTableModel * mdl_sql;
};

#endif // DIALOG_H
