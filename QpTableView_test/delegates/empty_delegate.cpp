#include "empty_delegate.h"
#include <QDebug>

Empty_Delegate::Empty_Delegate(QWidget *parent)
    :
      QStyledItemDelegate(parent)
{

}

void Empty_Delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    qDebug() << "Empty_Delegate::setModelData";
}

QWidget* Empty_Delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    qDebug() << "Empty_Delegate::createEditor";
    return 0;
}
