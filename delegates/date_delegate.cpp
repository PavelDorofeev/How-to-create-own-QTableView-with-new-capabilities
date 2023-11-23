#include "date_delegate.h"
#include <QDebug>

date_Delegate::date_Delegate(int fullDateTime, QWidget *parent)
{

    fullDateTime_=fullDateTime;
}

QWidget *date_Delegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &/* option */,
                                     const QModelIndex &/* index */) const
{
    QDateEdit *editor = new QDateEdit(parent);
    editor->setDisplayFormat("dd.MM.yy");
    return editor;
}
void date_Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDate value = index.model()->data(index, Qt::EditRole).toDate();

    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    dateEdit->setDate(value);
}
void date_Delegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    QDateTime dt = dateEdit->dateTime();
    uint se = dt.toTime_t();
    model->setData(index, se, Qt::EditRole);

}
void date_Delegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

QString date_Delegate::displayText(const QVariant &value, const QLocale &locale) const
{
    //value.toDateTime()
    QDateTime dt;
    dt.setTime_t(value.toInt());

    //qDebug() << "value " <<value.toString();
    //qDebug() << "dt.fromTime_t " << dt.toString("yyyy-MM-dd HH:MM:ss");

    if(fullDateTime_ == 0 )
        return locale.toString(dt, "yyyy-MM-dd");
    else
        return locale.toString(dt, "yyyy-MM-dd hh:mm");

}
