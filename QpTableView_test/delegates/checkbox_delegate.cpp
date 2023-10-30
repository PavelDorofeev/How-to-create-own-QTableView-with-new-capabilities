#include "checkbox_delegate.h"
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QPalette>
#include <QSvgRenderer>

#include "pbltableview/PblSqlRelationalTableModel.h"
#include "pbltableview/pbltableview.h"


checkBox_Delegate::checkBox_Delegate( QWidget *parent)
    :
      QStyledItemDelegate(parent)
{

    svg_renderer = new QSvgRenderer( QString(":icon/img/checked.svg") , this);

    svg_renderer_inv = new QSvgRenderer( QString(":icon/img/unchecked.svg") , this);


}

checkBox_Delegate::~checkBox_Delegate()
{
    delete svg_renderer;
    delete svg_renderer_inv;

    qDebug("~checkBox_Delegate");
}

QWidget *checkBox_Delegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    return 0;
}


void checkBox_Delegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{

    bool checked = index.model()->data(index, Qt::DisplayRole).toBool();


    QStyleOptionButton opt;

    //opt.state |= QStyle::State_Enabled;

    painter->save();

    QRect rect1 (option.rect.x() + PblTableView::margin_hor,
                 option.rect.y(),
                 option.rect.width() - (2 * PblTableView::margin_hor),
                 option.rect.height());

    //qDebug() <<"painter " <<painter->paintEngine()

    if( rect1.width() > rect1.height())
    {
        int offsetX = (rect1.width() - rect1.height()) / 2;
        rect1.setX(rect1.x() + offsetX );
        rect1.setWidth(rect1.height());
    }
    else
    {
        int offsetY = (rect1.height() - rect1.width()) / 2;
        rect1.setY(rect1.y() + offsetY );
        rect1.setHeight(rect1.width());
    }


    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt1 = option;

    initStyleOption(&opt1, index);

    QStyle *style = QApplication::style();


    opt1.text=""; //!!
    style->drawControl(QStyle::CE_ItemViewItem, &opt1, painter);

    if(index.row() ==1 )
    {
    }


    if (checked)
    {
        svg_renderer->render( painter , rect1);
    }
    else
    {
        svg_renderer_inv->render(painter, rect1);
    }


    painter->restore();

}
