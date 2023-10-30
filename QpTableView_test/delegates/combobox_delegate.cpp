#include "combobox_delegate.h"
#include <QComboBox>
#include <QDebug>
#include <QApplication>
#include <QStyledItemDelegate>
#include "private/qstylesheetstyle_p.h"
#include <QPainter>
#include <QPalette>
#include <QApplication>

ComboBoxDelegate::ComboBoxDelegate(
        const QStringList &Lst,
        QWidget *parent)
    :
      QStyledItemDelegate(parent),

      lst(Lst),
      parent_(parent)

{


}



QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{


    QComboBox *cmb = new QComboBox(parent);

    cmb->insertItems(0,lst);

    int width = 50;

    for(int ii=0; ii < cmb->count(); ii++)
    {
        if( width < QApplication::fontMetrics().width( cmb->itemText(ii)) )
            width = QApplication::fontMetrics().width( cmb->itemText(ii));
    }



    QPalette palette = option.palette; //editor->palette();

    cmb->setPalette(palette);

    QStyleOptionViewItem opt = option;

    cmb->setSizeAdjustPolicy( QComboBox::AdjustToContents );

    cmb->setMinimumWidth( width);

    cmb->updateGeometry();

    qDebug() << "cmb->geometry() " << cmb->geometry();
    return cmb;
}



void ComboBoxDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex &index) const
{

    QComboBox *edit = qobject_cast<QComboBox *>(editor);

    int newValue=edit->currentIndex();

    bool res = model->setData(index, newValue, Qt::EditRole);

}


QString ComboBoxDelegate::displayText(const QVariant &value,
                                      const QLocale &locale) const
{

    bool ok = false;
    int ii = value.toInt(&ok);

    if(ok)
    {
        if ( ii>=0 && ii <lst.count() )
        {
            return lst.at(ii);
        }
    }

    return "";
}

//void ComboBoxDelegate::paint(QPainter *painter,
//                             const QStyleOptionViewItem &option,
//                             const QModelIndex &index) const
//{


//    QStyleOptionViewItemV4 opt1 = option;

//    //initStyleOption(&opt1, index); // !!! set opt1.text =...

//    //QStyle *style = QApplication::style();

//    //style->drawControl(QStyle::CE_ItemViewItem, &opt1, painter);

//    //style->drawItemText( painter , opt1.rect , Qt::AlignCenter , option.palette, true, opt1.text );

//    //painter->drawText( opt1.rect ,  opt1.text);

//    //opt.rect.adjust(0, 0, 0, 0);

//    QStyleOptionViewItemV4 opt = option;
//    initStyleOption(&opt, index);

//    //const QWidget *widget = QStyledItemDelegatePrivate::widget(opt);

//    QStyle *style = parent_ ? parent_->style() : QApplication::style();

//    qDebug() << "paint opt "<< opt<< " opt.text:" << opt.text;

//    style->drawPrimitive(QStyle::PE_PanelItemViewRow, &opt, painter, parent_); // what is this?

//    //style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, parent_);
//    //QStyledItemDelegate::paint(painter, opt, index);

//}


