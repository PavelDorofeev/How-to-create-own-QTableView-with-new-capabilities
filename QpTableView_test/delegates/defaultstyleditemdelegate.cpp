#include "defaultstyleditemdelegate.h"

#include "pbltableview/PblSqlRelationalTableModel.h"
#include "pbltableview/pbltableview.h"

#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QPalette>
//#include <QStyleSheetStyle>
#include <QApplication>

DefaultStyledItemDelegate::DefaultStyledItemDelegate( QObject *parent) :
    QStyledItemDelegate(parent)
{

    //mdl = Mdl;
}

//QSize DefaultStyledItemDelegate::sizeHint(const QStyleOptionViewItem &option,
//                                          const QModelIndex &index) const
//{
//    //2 * QStyle::PM_FocusFrameHMargin
//    QSize sz = QStyledItemDelegate::sizeHint(option , index );

//    //sz.setWidth( sz.width() + PblTableView::margin_hor*2 );

//    return sz;
//}

void DefaultStyledItemDelegate::paint(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &idx) const
{
    if(idx.column() == 1)
    {
        //qDebug() << "\nDefaultStyledItemDelegate row " << idx.row()<< "  col " << idx.column() << " option.state " << option.state; // !!!

        //qDebug() << "painter " << painter->background();

        //qDebug() << "currentColorGroup " << option.palette.currentColorGroup();
    }


    QStyledItemDelegate::paint(painter, option, idx);


    //    QStyleOptionViewItem opt(option);

    //    int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

    //    QString text="";

    //    int row=idx.row();

    //    if( idx.isValid())
    //    {
    //        text = mdl->data(idx).toString();
    //    }
    //    //qDebug() << " textMargin " << textMargin;
    //    Qt::Alignment alignment = Qt::AlignCenter;

    //    if ( mdl->data(idx , Qt::TextAlignmentRole ).isValid() )
    //    {
    //        alignment = (Qt::Alignment)mdl->data(idx , Qt::TextAlignmentRole ).toInt();
    //    }


    //    painter->save();

    //    QRect rect1 (opt.rect.x() + PblTableView::margin_hor,
    //                        opt.rect.y(),
    //                        opt.rect.width() - (2 * PblTableView::margin_hor),
    //                        opt.rect.height());

    //    if(mdl->isSelectedLine != -1 && row == mdl->isSelectedLine)
    //    {

    //        painter->setPen(QPen(opt.palette.color(QPalette::Text))); // Цвет пера

    //        painter->fillRect( opt.rect, QApplication::palette().color(QPalette::AlternateBase));

    //        painter->drawText( rect1 , alignment, text);

    //    }
    //    else
    //    {
    //        if (opt.state & QStyle::State_Selected)
    //        {
    //            painter->setPen( QPen ( opt.palette.color(QPalette::HighlightedText))); // Цвет пера
    //            painter->fillRect( opt.rect, opt.palette.highlight());
    //        }
    //        else if (opt.state & (QStyle::State_Enabled ))
    //        {
    //            painter->setPen(QPen(opt.palette.color(QPalette::Text))); // Цвет пера

    //            painter->fillRect( opt.rect, opt.palette.base());

    //        }


    //        painter->drawText(rect1, alignment, text);


    //    }

    //    // последовательность drawComplexControl и drawControl ИМЕЕТ ЗНАЧЕНИЕ !!!

    //    painter->restore();

}

//QString DefaultStyledItemDelegate::displayText(const QVariant &value,
//                                               const QLocale &locale) const
//{
//    //qDebug() << QString("ComboBoxDelegate::displayText ")  << objectName() << "  value : " << value.toString();

//    QString str=value.toString();
//    return str;
//}
