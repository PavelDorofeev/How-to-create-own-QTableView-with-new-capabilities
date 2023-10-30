#include "icon_styleditemdelegate.h"
#include "pbltableview/pbltableview.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QPalette>
#include <QPicture>
#include <QSvgRenderer>

Icon_StyledItemDelegate::Icon_StyledItemDelegate(const QString & fileName1,
                                                 const QString & fileName2,
                                                 QObject *parent)
    :
      QStyledItemDelegate(parent)
{

    //image = Image;

    svg_renderer = new QSvgRenderer( fileName1 , this);
    svg_renderer_inv = new QSvgRenderer( fileName2 , this);

}
QWidget* Icon_StyledItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return 0;
}

QSize Icon_StyledItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    //2 * QStyle::PM_FocusFrameHMargin
    QSize sz = QStyledItemDelegate::sizeHint(option , index );

    QFont font = QApplication::font();

    QFontMetrics fm(font);

    int em1H = fm.height();

    QSize sz2 (em1H , em1H);

    //qDebug() << "  Icon_StyledItemDelegate::sizeHint " << sz << sz2;

    return sz;
}

void Icon_StyledItemDelegate::paint(QPainter *painter,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &idx) const
{

    QStyleOptionViewItem opt(option);

    QString text="";

    int col=idx.column();
    int row=idx.row();

    painter->save();

    Qt::Alignment alignment = Qt::AlignVCenter|Qt::AlignHCenter;

    QRect rect1 (opt.rect.x() + PblTableView::margin_hor,
                 opt.rect.y(),
                 opt.rect.width() - (2 * PblTableView::margin_hor),
                 opt.rect.height());



    if( rect1.width() > rect1.height())
    {
         int offsetX = (rect1.width() - rect1.height()) / 2;
         rect1.setX(rect1.x() + offsetX );
         rect1.setWidth(rect1.height());
        //rect1.set
    }
    else
    {
        int offsetY = (rect1.height() - rect1.width()) / 2;
        rect1.setY(rect1.y() + offsetY );
        rect1.setHeight(rect1.width());
    }


    if (opt.state & QStyle::State_Selected)
    {
        painter->setPen( QPen ( opt.palette.color(QPalette::HighlightedText))); // Цвет пера

        painter->fillRect( opt.rect, opt.palette.highlight());

        svg_renderer_inv->render(painter, rect1);
    }
    else if (opt.state & (QStyle::State_Enabled ))
    {
        painter->setPen(QPen(opt.palette.color(QPalette::Text))); // Цвет пера

        painter->fillRect(opt.rect, opt.palette.base());

        svg_renderer->render(painter, rect1);
    }



    //QPixmap pxmap(":icon/img/renewal.svg");
    //QPicture pct(":icon/img/renewal.svg");
    //painter->drawImage(rect1, image);
    //painter->drawPixmap(rect1, pxmap);

    //painter->drawPicture(pxmap);

    // последовательность drawComplexControl и drawControl ИМЕЕТ ЗНАЧЕНИЕ !!!

    painter->restore();


}
