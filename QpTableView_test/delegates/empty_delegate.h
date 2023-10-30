#ifndef EMPTY_DELEGATE_H
#define EMPTY_DELEGATE_H

#include <QStyledItemDelegate>

class Empty_Delegate : public QStyledItemDelegate
{
public:
    Empty_Delegate(QWidget *parent);

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
};

#endif // EMPTY_DELEGATE_H
