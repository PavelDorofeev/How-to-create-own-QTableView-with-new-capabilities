#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QStandardItemModel>
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
    
    void init();

private slots:

    void on_btn_Update_clicked();

    void on_btn_init_sections_2_clicked();

    void on_btn_init_sections_clicked();

    void on_btn_init_sections_3_clicked();

private:
    Ui::Dialog *ui;


    Qp_SECTION_TMPL prepare_matrix( const QPlainTextEdit & txt );

    QStandardItemModel * model;
};

#endif // DIALOG_H
