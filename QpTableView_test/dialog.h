#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include "qp/tableview/qp_horheaderview.h"

namespace Ui {
class Dialog;
}

class QpTableView;

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

    //static int lbl_number;
    void init_styles();


private slots:

    void slot_aaa(const QModelIndex& idx);

    void slot_settinggs_edit(const QPoint& pp);

    void on_btn_Update_clicked();

    void on_btn_init_sections_2_clicked();

    void on_btn_init_sections_clicked();

    void on_btn_init_sections_3_clicked();

    void on_btn_sqltableModel_On_clicked();

    void on_btn_QStandardItemModel_On_clicked();

    void on_chk_grid_on_clicked(bool checked);


    void on_btn_logFile_clicked();

    void on_cmb_SelectionBehavoir_activated(int index);

    void on_cmb_SelectionMode_activated(int index);

    void on_chk_betweenRowsBorder_clicked(bool checked);


    void on_cmb_SelectionMode_currentIndexChanged(int index);

    void on_cmb_SelectionBehavoir_currentIndexChanged(int index);

    void on_btn_insertRow_clicked();

    void on_btn_resize1_clicked();

private:
    Ui::Dialog *ui;


    qp::SECTIONS_TMPL prepare_matrix( const QPlainTextEdit & txt );

    QAbstractTableModel *mdl;
    QStandardItemModel * mdl_standart;
    QSqlTableModel * mdl_sql;

    QpTableView *view;
};

#endif // DIALOG_H
