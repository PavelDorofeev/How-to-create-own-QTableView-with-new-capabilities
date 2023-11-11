/******************************************************************************
**
** Contact: BIT Ltd Company (p@kkmspb.ru) Individual Taxpayer Number (ITN) 7826152874
**
** This file is not part of the Qt Sources.
** This is a little convenient fork of QTableView (Qt 4.8.1) version 4.0
** created by Pavel Dorofeev ( p@kkmspb.ru )
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.

*******************************************************************************/

#include "qp_tableview2.h"

#ifndef QT_NO_TABLEVIEW
#include <QtGui/qp/qp_horheaderview.h>
#include <QtGui/qp/qp_vertheaderview.h>
#include <qitemdelegate.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qsize.h>
#include <qevent.h>
#include <qbitarray.h>
#include <qscrollbar.h>
#include <qabstractbutton.h>
#include <QStyledItemDelegate>
#include <out/qp_tableview2_p.h>
#include <private/qabstractitemview_p.h>
#ifndef QT_NO_ACCESSIBILITY
#include <qaccessible.h>
#endif

//#include "common/dbg.h"

QT_BEGIN_NAMESPACE

const int QpTableView2::defaultRowHeight = 60;

const bool QpTableView2::debug = false;
const bool QpTableView2::debug_paint = true;
const bool QpTableView2::debug_init = false;
const bool QpTableView2::debug_event = false;
const bool QpTableView2::debug_paint_region = true;
const bool QpTableView2::debug_paint_row_col = true;
const bool QpTableView2::debug_paint_border = false;
const bool QpTableView2::debug_selection = true;
const bool QpTableView2::debug_scroll = false;
const bool QpTableView2Private::debug = false;
const bool QpTableView2Private::debug_init = true;
const bool QpTableView2Private::debug_selection = true;

const int QpTableView2::correct_width_minus_1 = -1;
const int QpTableView2::correct_height_minus_1 = -1;

class QTableCornerButton2 : public QAbstractButton
{
    Q_OBJECT
public:
    QTableCornerButton2(QWidget *parent) : QAbstractButton(parent) {}

    void paintEvent(QPaintEvent*)
    {
        QStyleOptionHeader opt;
        opt.init(this);
        QStyle::State state = QStyle::State_None;
        if (isEnabled())
            state |= QStyle::State_Enabled;
        if (isActiveWindow())
            state |= QStyle::State_Active;
        if (isDown())
            state |= QStyle::State_Sunken;
        opt.state = state;
        opt.rect = rect();
        opt.position = QStyleOptionHeader::OnlyOneSection;
        QPainter painter(this);
        style()->drawControl(QStyle::CE_Header, &opt, &painter, this);
    }
};


QpTableView2Private::QpTableView2Private()
    :
      QAbstractItemViewPrivate(),

      showGrid(true),
      gridStyle(Qt::SolidLine),
      rowSectionAnchor(-1),
      columnSectionAnchor(-1),
      columnResizeTimerID(0),
      rowResizeTimerID(0),
      horizontalHeader(0),
      verticalHeader(0),
      sortingEnabled(false),
      geometryRecursionBlock(false),
      visualCursor(QPoint()),
      showBetweenRowsBorder(true)
{
    wrapItemText = true;
#ifndef QT_NO_DRAGANDDROP
    overwrite = true;
#endif
}

QpTableView2Private::~QpTableView2Private()
{
}

//void QpTableView2Private::_q_rowsRemoved(const QModelIndex &parent, int start, int end)
//{

//}

//void QpTableView2Private::_q_rowsInserted(const QModelIndex &parent, int start, int end)
//{

//}

//void QpTableView2Private::_q_columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
//{

//}

//void QpTableView2Private::_q_columnsRemoved(const QModelIndex &parent, int start, int end)
//{
//    //
//}

//void QpTableView2Private::_q_columnsInserted(const QModelIndex &parent, int start, int end)
//{

//}

//bool QpTableView2Private::dropOn(QDropEvent *event, int *row, int *col, QModelIndex *index)
//{
//    return true;
//}

//void QpTableView2Private::_q_modelDestroyed()
//{

//}

//void QpTableView2Private::_q_layoutChanged()
//{

//}

void QpTableView2Private::selectAll(QItemSelectionModel::SelectionFlags command)
{

}

QItemViewPaintPairs QpTableView2Private::draggablePaintPairs(const QModelIndexList &indexes, QRect *r) const
{
    QItemViewPaintPairs lst;
    return lst;
}

QAbstractItemView::DropIndicatorPosition QpTableView2Private::position(const QPoint &pos, const QRect &rect, const QModelIndex &idx) const
{
    QAbstractItemView::DropIndicatorPosition pp;
    return pp;
}

void QpTableView2Private::init(  )
{
    Q_Q(QpTableView2);

    if ( debug_init ) qDebug() << "QpTableView2Private::init()" ;

    q->setEditTriggers( editTriggers|QAbstractItemView::AnyKeyPressed);

    QpHorHeaderView *horizontal = new QpHorHeaderView( Qt::Horizontal, q);

    horizontal->setClickable(true);
    horizontal->setHighlightSections(true);
    q->setHorizontalHeader(horizontal);

    QpVertHeaderView *vertical = new QpVertHeaderView( *horizontal, Qt::Vertical, q);
    vertical->setClickable(true);
    vertical->setHighlightSections(true);
    q->setVerticalHeader(vertical);


    tabKeyNavigation = true;

    cornerWidget = new QTableCornerButton2(q);
    cornerWidget->setFocusPolicy(Qt::NoFocus);
    QObject::connect(cornerWidget, SIGNAL(clicked()), q, SLOT(selectAll()));

}


void QpTableView2Private::trimHiddenSelections(QItemSelectionRange *range) const
{
    //if ( debug_selection ) qDebug() << "QpTableView2Private::trimHiddenSelections";

    //    Q_ASSERT(range && range->isValid());

    //    int top = range->top();
    //    int left = range->left();
    //    int bottom = range->bottom();
    //    int right = range->right();

    //    while (bottom >= top && verticalHeader->isSectionHidden(bottom))
    //        --bottom;

    //    while (right >= left && horizontalHeader->isSectionHidden(right))
    //        --right;

    //    if (top > bottom || left > right)
    //    { // everything is hidden
    //        *range = QItemSelectionRange();
    //        return;
    //    }

    //    while (verticalHeader->isSectionHidden(top) && top <= bottom)
    //        ++top;

    //    while (horizontalHeader->isSectionHidden(left) && left <= right)
    //        ++left;

    //    if (top > bottom || left > right)
    //    { // everything is hidden
    //        *range = QItemSelectionRange();
    //        return;
    //    }

    //    QModelIndex bottomRight = model->index(bottom, right, range->parent());
    //    QModelIndex topLeft = model->index(top, left, range->parent());

    //    *range = QItemSelectionRange(topLeft, bottomRight);
}

void QpTableView2Private::drawCellLabel(QPainter *painter, const QStyleOptionViewItemV4 &option,  const QString &txt, bool rowSelected, int row , const qp::LABEL_STYLE & st)
{
    Q_Q(QpTableView2);

    QStyleOptionViewItemV4 opt = option;

    opt.displayAlignment = st.align;

    // -------------------------------------------------------------
    // lifehack: label selection is detected by index (row,0)
    // -------------------------------------------------------------

    QModelIndex index = model->index( row, 0 , root);


    if(  selectionModel && q->selectionBehavior() == QAbstractItemView::SelectRows  &&  index.isValid() )
    {
        if (selectionModel && selectionModel->isSelected(index))
        {

            opt.state |= QStyle::State_Selected;
        }

    }

    //opt. |= QStyle::State_Active;


    if ( option.state & QStyle::State_Enabled )
    {

        QPalette::ColorGroup cg;

        //        if ((model->flags(index) & Qt::ItemIsEnabled) == 0)
        //        {
        //            opt.state &= ~QStyle::State_Enabled;
        //            cg = QPalette::Disabled;
        //        }
        //        else
        //        {
        cg = QPalette::Normal;
        //}
        opt.palette.setCurrentColorGroup(cg);
    }

    q->style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &opt, painter, q); // what is this?

    opt.text = txt;


    //QStyleOptionViewItemV4 opt = option;
    //initStyleOption(&opt, index);

    if (const QStyleOptionViewItemV4 *v4 = qstyleoption_cast<const QStyleOptionViewItemV4 *>(&opt))
    {
        const QWidget *widget = v4->widget;

        QStyle *style = widget ? widget->style() : QApplication::style();

        //QModelIndex index = model->index( row, 0);

        //QAbstractItemDelegate * del = q->itemDelegate(index);//->paint(painter, opt, index); // this paints text

        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

        //        if ( debug_selection )qDebug() <<"                    QpTableView2Private::drawCellLabel  drawControl row:"<<row<< " opt.text:"<< opt.text << " rowSelected "<< rowSelected;
        //        if ( debug_selection )qDebug() <<"                        opt.state " << opt.state;
        //        if ( debug_selection )qDebug() <<"                        widget " << widget;
    }
    else
        if ( debug_selection )qDebug()<<"sdafdsfasdfwe4rere";

}

void QpTableView2Private::drawCell(QPainter *painter, const QStyleOptionViewItemV4 &option, const QModelIndex &index)
{
    Q_Q(QpTableView2);

    QStyleOptionViewItemV4 opt = option;

    if (selectionModel && selectionModel->isSelected(index))
    {

        opt.state |= QStyle::State_Selected;
    }

//    if (index == hover)
//    {
//        opt.state |= QStyle::State_MouseOver;
//    }

    if (option.state & QStyle::State_Enabled)
    {
        QPalette::ColorGroup cg;

        if ((model->flags(index) & Qt::ItemIsEnabled) == 0)
        {
            opt.state &= ~QStyle::State_Enabled;
            cg = QPalette::Disabled;
        }
        else
        {
            cg = QPalette::Normal;
        }
        opt.palette.setCurrentColorGroup(cg);
    }

    if (index == q->currentIndex())
    {
        const bool focus = (q->hasFocus() || viewport->hasFocus()) && q->currentIndex().isValid();

        if (focus)
            opt.state |= QStyle::State_HasFocus;
    }


    if ( debug_selection ) qDebug() << "QpTableView2Private::drawCell row :"<< index.row()<<":" <<index.column() << " opt " << opt ;

    q->style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &opt, painter, q); // what is this?

    if ( debug_selection )qDebug() <<"                    QpTableView2Private::drawCell  drawControl  opt.text:"<< opt.text << " index " << index;
    if ( debug_selection )qDebug() <<"                        opt.state " << opt.state;
    //qDebug() <<"                        widget " << widget;

    q->itemDelegate(index)->paint(painter, opt, index); // this paints text
}


QpTableView2::QpTableView2( QWidget *parent, Qp_SECTION_TMPL *matrix)
    :
      QAbstractItemView(*new QpTableView2Private, parent),
      delayed_Repaint_tmr(0),
      row_selected(-1)
{
    Q_D(QpTableView2);

    d->init( );

    //viewport()->setBackgroundRole( QPalette::Button );
    //setBackgroundRole( QPalette::Button ); //!!
    if(debug) qDebug() << "ctor QpTableView2::QTableView2(QWidget *parent): QAbstractItemView(*new QpTableView2Private, parent) backgroundRole:" <<  backgroundRole();

}

QpTableView2::QpTableView2(QpTableView2Private &dd,  Qp_SECTION_TMPL *matrix, QWidget *parent)
    : QAbstractItemView(dd, parent),
      row_selected(-1)
{
    Q_D(QpTableView2);


    d->init( );

    if( debug ) qDebug() << "ctor QTableView2 (QpTableView2Private&, QWidget *) backgroundRole:" << backgroundRole();
    //viewport()->setBackgroundRole( QPalette::Button );
    //setBackgroundRole( QPalette::Button ); //!!

}


QpTableView2::~QpTableView2()
{
}


void QpTableView2::setModel (QAbstractItemModel *mdl , const Qp_SECTION_TMPL & matrix)
{
    Q_D(QpTableView2);


    if (mdl == model())
        return;


    //let's disconnect from the old model
    if (model() && model() != QAbstractItemModelPrivate::staticEmptyModel())
    {
        disconnect(model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this, SLOT(slot_rowsInserted(QModelIndex,int,int)));

        //                disconnect(model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
        //                           this, SLOT(_q_updateSpanInsertedRows(QModelIndex,int,int)));
        //                disconnect(model(), SIGNAL(columnsInserted(QModelIndex,int,int)),
        //                           this, SLOT(_q_updateSpanInsertedColumns(QModelIndex,int,int)));
        //                disconnect(model(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
        //                           this, SLOT(_q_updateSpanRemovedRows(QModelIndex,int,int)));
        //                disconnect(model(), SIGNAL(columnsRemoved(QModelIndex,int,int)),
        //                           this, SLOT(_q_updateSpanRemovedColumns(QModelIndex,int,int)));
    }
    if (mdl)
    { //and connect to the new one
        qDebug() << "QpTableView2::setModel";

        connect(mdl, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(slot_rowsInserted(QModelIndex,int,int)));

        //                connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
        //                        this, SLOT(_q_updateSpanInsertedRows(QModelIndex,int,int)));
        //                connect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),
        //                        this, SLOT(_q_updateSpanInsertedColumns(QModelIndex,int,int)));
        //                connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
        //                        this, SLOT(_q_updateSpanRemovedRows(QModelIndex,int,int)));
        //                connect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
        //                        this, SLOT(_q_updateSpanRemovedColumns(QModelIndex,int,int)));
    }

    d->horizontalHeader->setModel( mdl , matrix); // !!


    d->verticalHeader->setModel( mdl );

    QAbstractItemView::setModel(mdl);
}

void QpTableView2::setRootIndex(const QModelIndex &index)
{
    Q_D(QpTableView2);

    if (index == rootIndex())
    {
        viewport()->update();
        return;
    }
    d->verticalHeader->setRootIndex(index);

    d->horizontalHeader->setRootIndex(index);

    QAbstractItemView::setRootIndex(index);
}


void QpTableView2::doItemsLayout()
{
    Q_D(QpTableView2);
    QAbstractItemView::doItemsLayout();

    if( debug_scroll) qDebug() << "QpTableView2::doItemsLayout() verticalScrollBar()->value()" << verticalScrollBar()->value();

    if (verticalScrollMode() == QAbstractItemView::ScrollPerItem)
        d->verticalHeader->setOffsetToSectionPosition(verticalScrollBar()->value());
    else
        d->verticalHeader->setOffset(verticalScrollBar()->value());

    if (!d->verticalHeader->updatesEnabled())
        d->verticalHeader->setUpdatesEnabled(true);
}

/*!
  \reimp
*/
void QpTableView2::setSelectionModel(QItemSelectionModel *selectionModel)
{
    qDebug() << "QpTableView2::setSelectionModel "<<selectionModel;

    Q_D(QpTableView2);

    Q_ASSERT(selectionModel);

    d->verticalHeader->setSelectionModel(selectionModel);

    d->horizontalHeader->setSelectionModel(selectionModel);

    QAbstractItemView::setSelectionModel(selectionModel);
}


QpHorHeaderView *QpTableView2::horizontalHeader() const
{
    Q_D(const QpTableView2);
    return d->horizontalHeader;
}


QpVertHeaderView *QpTableView2::verticalHeader() const
{
    Q_D(const QpTableView2);

    return d->verticalHeader;
}


void QpTableView2::setHorizontalHeader(QpHorHeaderView *header)
{
    Q_D(QpTableView2);

    if (!header || header == d->horizontalHeader)
        return;

    if (d->horizontalHeader && d->horizontalHeader->parent() == this)
        delete d->horizontalHeader;

    d->horizontalHeader = header;
    d->horizontalHeader->setParent(this);

    if ( !d->horizontalHeader->model())
    {
        d->horizontalHeader->setModel( model() , horizontalHeader()->get_matrix());

        if (d->selectionModel)
            d->horizontalHeader->setSelectionModel(d->selectionModel);
    }

    connect(d->horizontalHeader,SIGNAL(sectionResized_Y()),
            this, SLOT(columnResized_Y())); //!!;

    connect(d->horizontalHeader,SIGNAL(sectionResized(int,int,int)),
            this, SLOT(columnResized(int,int,int)));

    connect(d->horizontalHeader, SIGNAL(sectionMoved(int,int,int)),
            this, SLOT(columnMoved(int,int,int)));

    connect(d->horizontalHeader, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(columnCountChanged(int,int)));

    connect(d->horizontalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectColumn(int)));

    connect(d->horizontalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectColumn(int)));

    connect(d->horizontalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
            this, SLOT(resizeColumnToContents(int)));

    connect(d->horizontalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()));

    //update the sorting enabled states on the new header
    setSortingEnabled(d->sortingEnabled);
}


void QpTableView2::setVerticalHeader(QpVertHeaderView *header)
{
    Q_D(QpTableView2);

    if (!header || header == d->verticalHeader)
        return;

    if (d->verticalHeader && d->verticalHeader->parent() == this)
        delete d->verticalHeader;

    d->verticalHeader = header;
    d->verticalHeader->setParent(this);

    if (!d->verticalHeader->model())
    {
        d->verticalHeader->setModel(model());

        if (d->selectionModel)
            d->verticalHeader->setSelectionModel(d->selectionModel);
    }

    connect(d->verticalHeader, SIGNAL(sectionResized(int,int,int)),
            this, SLOT(rowResized(int,int,int)));

    connect(d->verticalHeader, SIGNAL(sectionMoved(int,int,int)),
            this, SLOT(rowMoved(int,int,int)));

    connect(d->verticalHeader, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(rowCountChanged(int,int)));

    connect(d->verticalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectRow(int)));

    connect(d->verticalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectRow(int)));

    connect(d->verticalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
            this, SLOT(resizeRowToContents(int)));

    connect(d->verticalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()));
}


void QpTableView2::scrollContentsBy(int dx, int dy)
{
    Q_D(QpTableView2);

    // dx это шаг в направлении - вправо, + влево

    if( debug_scroll ) qDebug() << "QpTableView2::scrollContentsBy dx " << dx << " dy " <<dy;

    //!!d->delayedAutoScroll.stop(); // auto scroll was canceled by the user scrolling

    //dx = isRightToLeft() ? -dx : dx;

    if (dx)
    {
        if ( horizontalScrollMode() == QAbstractItemView::ScrollPerItem )
        {
            int oldOffset = d->horizontalHeader->offset();

            int max = horizontalScrollBar()->maximum();

            if( debug_scroll ) qDebug() <<" horizontalScrollBar()->value() "<<horizontalScrollBar()->value() << "  horizontalScrollBar()->maximum():"<< max;

            int value = horizontalScrollBar()->value();

            if ( value == max )
                d->horizontalHeader->setOffsetToLastSection();
            else
                d->horizontalHeader->setOffsetToSectionPosition( value );

            int newOffset = d->horizontalHeader->offset();

            dx = oldOffset - newOffset;
        }
        else
        {
            d->horizontalHeader->setOffset(horizontalScrollBar()->value());
        }
    }

    if (dy)
    {
        if (verticalScrollMode() == QAbstractItemView::ScrollPerItem)
        {
            int oldOffset = d->verticalHeader->offset();

            if( debug_scroll ) qDebug() << "       verticalScrollBar()->value() " <<verticalScrollBar()->value();

            if( debug_scroll ) qDebug() << "       verticalScrollBar()->maximum() " <<verticalScrollBar()->maximum();

            if (verticalScrollBar()->value() == verticalScrollBar()->maximum())
            {
                qDebug() << "setOffsetToLastSection()";
                d->verticalHeader->setOffsetToLastSection();
            }
            else
            {
                int visualY = verticalScrollBar()->value();

                if( debug_scroll ) qDebug() << "setOffsetToSectionPosition("<<visualY <<")";

                d->verticalHeader->setOffsetToSectionPosition(visualY );
            }


            int newOffset = d->verticalHeader->offset();

            dy = oldOffset - newOffset;
        }
        else
        {
            d->verticalHeader->setOffset(verticalScrollBar()->value());
        }
    }

    d->scrollContentsBy(dx, dy); // inline

    if (d->showGrid)
    {
        //we need to update the first line of the previous top item in the view
        //because it has the grid drawn if the header is invisible.
        //It is strictly related to what's done at then end of the paintEvent
        if (dy > 0 && d->horizontalHeader->isHidden() && verticalScrollMode() == ScrollPerItem)
        {
            viewport()->update(0, dy, viewport()->width(), dy);
        }
        if (dx > 0 && d->verticalHeader->isHidden() && horizontalScrollMode() == ScrollPerItem)
        {
            viewport()->update(dx, 0, dx, viewport()->height());
        }
    }
}

/*!
  \reimp
*/
QStyleOptionViewItem QpTableView2::viewOptions() const
{
    QStyleOptionViewItem option = QAbstractItemView::viewOptions();

    option.showDecorationSelected = true;

    return option;
}

/*!
    Paints the table on receipt of the given paint event \a event.
*/
void QpTableView2::paintEvent(QPaintEvent *event)
{
    Q_D(QpTableView2);
    // setup temp variables for the painting

    if( debug ) qDebug()<<"\n---------------------------------------------------------------------------------------------\n                QpTableView2::paintEvent \n---------------------------------------------------------------------------------------------\n";

    QStyleOptionViewItemV4 option = viewOptions();// d->viewOptionsV4();

    const QPoint offset = dirtyRegionOffset();// d->scrollDelayOffset;

    const bool showGrid = d->showGrid;


    const int gridSize = showGrid ? 1 : 0;

    const int gridHint = style()->styleHint(QStyle::SH_Table_GridLineColor, &option, this);

    const bool  betweenRowsBorder = d->showBetweenRowsBorder;

    //const QColor gridColor = static_cast<QRgb>(gridHint);
    const QColor gridColor = QColor(Qt::green);

    const QPen gridPen = QPen(gridColor, 0, d->gridStyle);

    const QpVertHeaderView *verticalHeader = d->verticalHeader;

    const QpHorHeaderView *horizontalHeader = d->horizontalHeader;

    const bool alternate = alternatingRowColors();//d->alternatingColors;

    const bool rightToLeft = isRightToLeft();

    QPainter painter(viewport());

    if (horizontalHeader->count() == 0 || verticalHeader->count() == 0 || ! d->delegateForIndex(QModelIndex()))// !!d->itemDelegate) //??
        return;

    uint x = horizontalHeader->length();
    x -= horizontalHeader->offset();

    int len_y = verticalHeader->length();
    uint y = len_y - verticalHeader->offset() - 1;


    if (  debug_paint_region)  qDebug() << "event->region():" << event->region();


    const QRegion region = event->region().translated(offset);

    const QVector<QRect> rects = region.rects();

    //    if( debug_paint ) qDebug() << "-----------------------------------------------";
    //    if( debug_paint ) qDebug() << "QpTableView2::paintEvent ";

    if( debug_paint_region ) qDebug() << "-----------------------------------------------";

    if( rects.count()>1)
        if ( debug_paint_region ) qDebug() << " !!!!!!!!!!!!!!!!!!!! rects count !!!!!!!!!!!!!!!! " <<rects.count();

    foreach( QRect rect ,rects )
    {
        if ( debug_paint_region )  qDebug() << "       region.rect()" << rect;
    }

    if( debug_paint_region ) qDebug() << "-----------------------------------------------";

    int firstVisualRow = qMax( verticalHeader->visualIndexAt( 0 ) , 0);

    int lastVisualRow = verticalHeader->visualIndexAt(verticalHeader->viewport()->height() );

    if (lastVisualRow == -1)
        lastVisualRow = model()->rowCount(rootIndex()) - 1;

    // ------------------------------------------------------------------------

    //int firstVisualColumn = horizontalHeader->visualIndexAt(0 , 0);
    int firstVisual_xNum = 0 ; // !!

    int rowH = verticalHeader->defaultSectionSize();

    int lastVisual_xNum = horizontalHeader->visualIndexAt_end( horizontalHeader->viewport()->width() );

    if (firstVisual_xNum == -1)
        firstVisual_xNum = 0;

    if (lastVisual_xNum == -1)
        lastVisual_xNum =  horizontalHeader->xNum_count() - 1;


    int lines_count = horizontalHeader->lines_count();

    int num_y_cnt =  model()->rowCount()  * lines_count;

    int num_x_cnt =  horizontalHeader->model()->columnCount() ;

    int sz = num_y_cnt * num_x_cnt ;

    int lgcl_max_num = horizontalHeader->lastLogicalNum() + 1;

    QBitArray drawn( sz );

    qDebug() << "QBitArray drawn " << drawn.size() << " num_y_cnt:"<<num_y_cnt << " num_x_cnt:"<<num_x_cnt;

    //drawn.resize( );

    // -------------------------------------------------------------------------------

    if ( debug_paint ) qDebug() << "QBitArray drawn : " << drawn;

    QString str = "\n\t\t";

    for( int ii =0; ii < drawn.count(); ii++)
    {
        int dd = ii % 10 ;

        if( dd == 0)
            str.append("\n\t\t");

        str.append( QString::number(drawn.at( ii )) ).append(" ");

    }

    if ( debug_paint ) qDebug() << str <<"\n";

    // -------------------------------------------------------------------------------

    for (int i = 0; i < rects.size(); ++i)
    {
        QRect dirtyArea = rects.at(i);

        if( debug_paint_region ) qDebug() << "dirtyArea rect: " << dirtyArea << "  firstVisualRow:" << firstVisualRow;

        dirtyArea.setBottom( qMin(dirtyArea.bottom(), int( y )));

        dirtyArea.setRight( qMin(dirtyArea.right(), int( x )));

        //int left = horizontalHeader->visualIndexAt( dirtyArea.left() , 0);
        int left_num = horizontalHeader->visual_xNum_At( dirtyArea.left() );

        //int right = horizontalHeader->visualIndexAt_end( dirtyArea.right()  );
        int right_num = horizontalHeader->visual_xNum_At( dirtyArea.right()  );


        if (left_num == -1)
            left_num = 0;

        if (right_num == -1)
            right_num = horizontalHeader->xNum_count() - 1;


        // get the vertical start and end visual sections and if alternate color
        int bottom = verticalHeader->visualIndexAt( dirtyArea.bottom() );

        if (bottom == -1)
            bottom = verticalHeader->count() - 1;

        int top = 0;

        bool alternateBase = false;

        top = verticalHeader->visualIndexAt(dirtyArea.top() );
        //alternateBase = (top & 1) && alternate;

        if (top == -1 || top > bottom)
            continue;

        //----------------------------------------------------
        //              Paint each row item
        //----------------------------------------------------

        //QPen old = painter.pen();
        QPainter painterGrid (viewport());

        painterGrid.setPen(gridPen);

        int section_count = horizontalHeader->count_of_section();

        for ( int row = top; row <= bottom; row++)
        {
            // --------------------------------------------------------------------
            //                      rows
            // --------------------------------------------------------------------

            if( debug_paint_row_col ) qDebug()<< "    row:"<<row<<" top:"<<top << " row - top  : " << row - top  << " lgcl_max_num:" << lgcl_max_num ;

            for ( int line = 0 ; line < lines_count; ++line)
            {
                // -------------------------------------------
                //                  lines
                // -------------------------------------------


                int rowY = rowViewportPosition( row );

                if( debug_paint_row_col ) qDebug()<< "        line:"<< line; ;

                rowY += offset.y();

                int row_cnt = row - firstVisualRow;

                int numY = row_cnt * lines_count +  line ;


                // ------------------------------------------------------------
                //                  Paint each cell item
                // ------------------------------------------------------------

                for (int numX = left_num; numX <= right_num; ++numX)
                {

                    int logicalIndex = horizontalHeader->logicalIndex_atNum_x_line( numX , line );

                    if( logicalIndex == qp::UNDEFINED_FLD)
                        continue;

                    QRect rect;

                    if( logicalIndex == qp::LABEL_FLD)
                    {

                        rect = horizontalHeader->cellPosition( line , numX );

                        int x2 = rect.right();
                        int y1 = rect.top();
                        int y2 = rect.bottom();

                        rect.setBottom( y2 - gridSize + correct_height_minus_1 );
                        rect.setRight(  x2 - gridSize + correct_width_minus_1 );

                        rect.moveTop( rowY + y1 );

                        option.rect = rect ;

                        QVariant var = horizontalHeader->cellLabelValue( line , numX );

                        //bool rowSel = is_RowSelected( row );
                        if( selectionBehavior()  == QAbstractItemView::SelectRows)
                        {

                        }
                        bool rowSel = verticalHeader->is_rowSelected( row );


                        if( rowSel )
                        {
                            qDebug() <<"        rowSel " << rowSel << " row "<< row;
                        }

                        if( debug_paint_row_col ) qDebug() << "               numX:"  << numX << "  label : "<< var.toString().toUtf8();

                        if( debug_paint_row_col )
                            qDebug() << "                   option.rect " << option.rect;

                        const qp::LABEL_STYLE style = horizontalHeader->get_label_style( line , numX );

                        d->drawCellLabel( &painter, option, var.toString() , rowSel , row , style);

                    }
                    else if (logicalIndex >=0 )
                    {
                        if ( horizontalHeader->isSectionHidden( logicalIndex ))
                            continue;

                        int currentBit = ( row - firstVisualRow ) * lgcl_max_num  + logicalIndex  ;

                        if ( currentBit < 0 || currentBit >= drawn.size() )// || drawn.testBit(currentBit))
                        {
                            int sz = drawn.size();
                            continue;
                        }


                        if(drawn.testBit (currentBit) )
                        {
                            continue;
                        }

                        drawn.setBit( currentBit );

                        //int rowh = rowHeight(row) - gridSize; // 99


                        rect =  columnViewportPosition2( logicalIndex );

                        if ( ! rect.isValid() )
                        {
                            qDebug() << "wrong rect for logicalIndex:"<<logicalIndex;
                            continue;
                        }

                        int x1=rect.left();
                        int x2=rect.right();
                        int y1=rect.top();
                        int y2=rect.bottom() ;

                        rect.setBottom( y2 - gridSize + correct_height_minus_1 );
                        rect.setRight(  x2 - gridSize + correct_width_minus_1 );


                        const QModelIndex index = model()->index( row, logicalIndex , rootIndex());


                        if ( ! index.isValid())
                        {
                            qDebug() << "! index.isValid() index " << index <<  model()->data( index).toString() << "  line " << line << " logicalIndex " << logicalIndex << " numX " << numX;
                            continue;
                        }

                        if( debug_paint_row_col ) qDebug() << "               numX:"  << numX << "  col : "<<logicalIndex;
                        rect.moveTop( rowY + y1 );

                        option.rect = rect ;

                        if (alternate)
                        {
                            if (alternateBase)
                                option.features |= QStyleOptionViewItemV2::Alternate;
                            else
                                option.features &= ~QStyleOptionViewItemV2::Alternate;
                        }

                        if( debug_paint_row_col )
                            qDebug() << "                   option.rect " << option.rect <<  model()->data( index).toString();


                        //-------------------------------------------------------------
                        d->drawCell( &painter, option, index);
                        //-------------------------------------------------------------
                    }



                    if( betweenRowsBorder )
                    {
                        //-------------------------------------------------------------
                        //  between rows border
                        //-------------------------------------------------------------

                        bool draw_bottom = false;

                        if( logicalIndex >=0 )
                        {
                            qp::CELL_NODES nd = horizontalHeader->get_logicalIdex_nodes( logicalIndex );

                            if( nd.bottom == lines_count  )
                            {
                                draw_bottom = true;
                            }

                        }

                        if ( logicalIndex == qp::LABEL_FLD)
                        {
                            if( line == lines_count - 1)
                            {
                                draw_bottom = true;
                            }
                        }

                        if( draw_bottom )
                        {
                            int gridWidth = horizontalHeader->gridWidth();

                            int xx1 = rect.left();

                            int xx2 = rect.right() + gridSize; // that is plus width
                            int yy2 = rect.bottom() + gridSize;


                            // hor
                            painterGrid.drawLine( xx1 ,  yy2 ,
                                                  xx2  , yy2);
                        }
                    }

                    if ( showGrid )
                    {
                        int gridWidth = horizontalHeader->gridWidth();

                        int xx1 = rect.left();
                        int yy1 = rect.top();
                        int xx2 = rect.right() + gridSize; // that is plus width
                        int yy2 = rect.bottom() + gridSize;

                        if(debug_paint_border)
                        {
                            QString str2 = QString("\n\n"\
                                                   "     %3              %4 \n"\
                                                   "   %1                   \n"\
                                                   "     |               |  \n"\
                                                   "     |               |  \n"
                                                   "   %2_________________  \n"
                                                   )
                                    .arg(yy1)
                                    .arg(yy2)
                                    .arg(xx1)
                                    .arg(xx2);

                            qDebug() << str2;
                        }


                        // ver
                        painterGrid.drawLine( xx2 , yy1 ,
                                              xx2 , yy2 );

                        // hor
                        if( ! betweenRowsBorder )
                        {

                            painterGrid.drawLine( xx1 ,  yy2 ,
                                                  xx2  , yy2);
                        }
                        else if( line < lines_count -1 )
                        {
                            painterGrid.drawLine( xx1 ,  yy2 ,
                                                  xx2  , yy2);

                        }

                    }

                }
                //alternateBase = ! alternateBase && alternate;
            }

        }
    }
#ifndef QT_NO_DRAGANDDROP
    // Paint the dropIndicator
    d->paintDropIndicator(&painter);
#endif

}

/*!
            Returns the index position of the model item corresponding to the
            table item at position \a pos in contents coordinates.
        */
QModelIndex QpTableView2::indexAt(const QPoint &pos) const
{
    Q_D(const QpTableView2);

    // ------------------------------------------------------
    //    this is called from user interactive actions only
    // ------------------------------------------------------

    ////d->executePostedLayout(); // !!

    int row = rowAt( pos.y()); // ok

    //int line = d->horizontalHeader->get_section_line( pos.y() );

    int rowH = rowHeight( row );

    int x = pos.x();
    int y = pos.y();

    int col = columnAt( x , y ) ; // columnAt uses only here (nothing anymore)

    //if( debug ) qDebug() << "   TableView2::indexAt(pos"<<pos<<")  r:"<<row << "  c:"<<col;

    if (row >= 0 && col >= 0)
    {
        QModelIndex idx = model()->index(row, col, rootIndex());

        if( debug_selection ) qDebug() << "   TableView2::indexAt pos "<<pos << " idx " << idx;

        return idx;
    }

    return QModelIndex();
}

/*!
            Returns the horizontal offset of the items in the table view.

            Note that the table view uses the horizontal header section
            positions to determine the positions of columns in the view.

            \sa verticalOffset()
        */
int QpTableView2::horizontalOffset() const
{
    Q_D(const QpTableView2);

    return d->horizontalHeader->offset();
}

/*!
            Returns the vertical offset of the items in the table view.

            Note that the table view uses the vertical header section
            positions to determine the positions of rows in the view.

            \sa horizontalOffset()
        */
int QpTableView2::verticalOffset() const
{
    Q_D(const QpTableView2);
    return d->verticalHeader->offset();
}

/*!
            \fn QModelIndex QpTableView2::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)

            Moves the cursor in accordance with the given \a cursorAction, using the
            information provided by the \a modifiers.

            \sa QAbstractItemView::CursorAction
        */
QModelIndex QpTableView2::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_D(QpTableView2);
    Q_UNUSED(modifiers);

    qDebug() << "QpTableView2::moveCursor cursorAction: " <<  cursorAction;

    int bottom = model()->rowCount(rootIndex()) - 1;
    // make sure that bottom is the bottommost *visible* row
    while (bottom >= 0 && isRowHidden(d->logicalRow(bottom)))
        --bottom;

    int right = model()->columnCount(rootIndex()) - 1;

    while (right >= 0 && isColumnHidden(d->logicalColumn(right)))
        --right;

    if (bottom == -1 || right == -1)
        return QModelIndex(); // model is empty

    QModelIndex current = currentIndex();

    if ( ! current.isValid())
    {
        int row = 0;
        int column = 0;

        while (column < right && isColumnHidden(d->logicalColumn(column)))
            ++column;

        while (isRowHidden(d->logicalRow(row)) && row < bottom)
            ++row;

        d->visualCursor = QPoint(column, row);

        return model()->index(d->logicalRow(row), d->logicalColumn(column), rootIndex());
    }

    // Update visual cursor if current index has changed.
    QPoint visualCurrent(d->visualColumn(current.column()), d->visualRow(current.row()));
    if (visualCurrent != d->visualCursor)
    {
        //        if (d->hasSpans()) {
        //            QSpanCollection2::Span span = d->span(current.row(), current.column());
        //            if (span.top() > d->visualCursor.y() || d->visualCursor.y() > span.bottom()
        //                    || span.left() > d->visualCursor.x() || d->visualCursor.x() > span.right())
        //                d->visualCursor = visualCurrent;
        //        } else {
        d->visualCursor = visualCurrent;
        //}
    }

    int visualRow = d->visualCursor.y();

    if (visualRow > bottom)
        visualRow = bottom;

    Q_ASSERT(visualRow != -1);

    int visualColumn = d->visualCursor.x();

    if (visualColumn > right)
        visualColumn = right;

    Q_ASSERT(visualColumn != -1);

    //    if (isRightToLeft())
    //    {
    //        if (cursorAction == MoveLeft)
    //            cursorAction = MoveRight;
    //        else if (cursorAction == MoveRight)
    //            cursorAction = MoveLeft;
    //    }

    switch (cursorAction)
    {
    case MoveUp: {
        int originalRow = visualRow;
#ifdef QT_KEYPAD_NAVIGATION
        if (QApplication::keypadNavigationEnabled() && visualRow == 0)
            visualRow = d->visualRow(model()->rowCount() - 1) + 1;
        // FIXME? visualRow = bottom + 1;
#endif
        int r = d->logicalRow(visualRow);
        int c = d->logicalColumn(visualColumn);

        //        if (r != -1 && d->hasSpans())
        //        {
        //            QSpanCollection2::Span span = d->span(r, c);
        //            if (span.width() > 1 || span.height() > 1)
        //                visualRow = d->visualRow(span.top());
        //        }

        while (visualRow >= 0)
        {
            --visualRow;
            r = d->logicalRow(visualRow);
            c = d->logicalColumn(visualColumn);
            if (r == -1 || (!isRowHidden(r) && d->isCellEnabled(r, c)))
                break;
        }

        if (visualRow < 0)
            visualRow = originalRow;
        break;
    }
    case MoveDown:
    {
        int originalRow = visualRow;
        //        if (d->hasSpans())
        //        {
        //            QSpanCollection2::Span span = d->span(current.row(), current.column());
        //            visualRow = d->visualRow(d->rowSpanEndLogical(span.top(), span.height()));
        //        }
#ifdef QT_KEYPAD_NAVIGATION
        if (QApplication::keypadNavigationEnabled() && visualRow >= bottom)
            visualRow = -1;
#endif
        int r = d->logicalRow(visualRow);
        int c = d->logicalColumn(visualColumn);

        //        if (r != -1 && d->hasSpans())
        //        {
        //            QSpanCollection2::Span span = d->span(r, c);
        //            if (span.width() > 1 || span.height() > 1)
        //                visualRow = d->visualRow(d->rowSpanEndLogical(span.top(), span.height()));
        //        }
        while (visualRow <= bottom)
        {
            ++visualRow;
            r = d->logicalRow(visualRow);
            c = d->logicalColumn(visualColumn);
            if (r == -1 || (!isRowHidden(r) && d->isCellEnabled(r, c)))
                break;
        }
        if (visualRow > bottom)
            visualRow = originalRow;
        break;
    }
    case MovePrevious:
    case MoveLeft:
    {
        int originalRow = visualRow;
        int originalColumn = visualColumn;
        bool firstTime = true;
        bool looped = false;
        bool wrapped = false;
        do {

            int r = d->logicalRow(visualRow);
            int c = d->logicalColumn(visualColumn);

            while (visualColumn >= 0)
            {
                --visualColumn;
                r = d->logicalRow(visualRow);
                c = d->logicalColumn(visualColumn);
                if (r == -1 || c == -1 || (!isRowHidden(r) && !isColumnHidden(c) && d->isCellEnabled(r, c)))
                    break;
                if (wrapped && (originalRow < visualRow || (originalRow == visualRow && originalColumn <= visualColumn))) {
                    looped = true;
                    break;
                }
            }
            if (cursorAction == MoveLeft || visualColumn >= 0)
                break;
            visualColumn = right + 1;
            if (visualRow == 0) {
                wrapped = true;
                visualRow = bottom;
            } else {
                --visualRow;
            }
        } while (!looped);
        if (visualColumn < 0)
            visualColumn = originalColumn;
        break;
    }
    case MoveNext:
    case MoveRight:
    {
        int originalRow = visualRow;
        int originalColumn = visualColumn;
        bool firstTime = true;
        bool looped = false;
        bool wrapped = false;
        do {
            int r = d->logicalRow(visualRow);
            int c = d->logicalColumn(visualColumn);

            //            if (firstTime && c != -1 && d->hasSpans())
            //            {
            //                firstTime = false;
            ////                QSpanCollection2::Span span = d->span(r, c);

            ////                if (span.width() > 1 || span.height() > 1)
            ////                    visualColumn = d->visualColumn(d->columnSpanEndLogical(span.left(), span.width()));
            //            }

            while (visualColumn <= right)
            {
                ++visualColumn;
                r = d->logicalRow(visualRow);
                c = d->logicalColumn(visualColumn);
                if (r == -1 || c == -1 || (!isRowHidden(r) && !isColumnHidden(c) && d->isCellEnabled(r, c)))
                    break;
                if (wrapped && (originalRow > visualRow || (originalRow == visualRow && originalColumn >= visualColumn))) {
                    looped = true;
                    break;
                }
            }

            if (cursorAction == MoveRight || visualColumn <= right)
                break;

            visualColumn = -1;
            if (visualRow == bottom)
            {
                wrapped = true;
                visualRow = 0;
            }
            else
            {
                ++visualRow;
            }
        } while (!looped);
        if (visualColumn > right)
            visualColumn = originalColumn;
        break;
    }
    case MoveHome:
        visualColumn = 0;
        while (visualColumn < right && d->isVisualColumnHiddenOrDisabled(visualRow, visualColumn))
            ++visualColumn;
        if (modifiers & Qt::ControlModifier) {
            visualRow = 0;
            while (visualRow < bottom && d->isVisualRowHiddenOrDisabled(visualRow, visualColumn))
                ++visualRow;
        }
        break;
    case MoveEnd:
        visualColumn = right;
        if (modifiers & Qt::ControlModifier)
            visualRow = bottom;
        break;
    case MovePageUp: {
        int newRow = rowAt(visualRect(current).top() - viewport()->height());
        if (newRow == -1)
            newRow = d->logicalRow(0);
        return model()->index(newRow, current.column(), rootIndex());
    }
    case MovePageDown: {
        int newRow = rowAt(visualRect(current).bottom() + viewport()->height());
        if (newRow == -1)
            newRow = d->logicalRow(bottom);
        return model()->index(newRow, current.column(), rootIndex());
    }}

    d->visualCursor = QPoint(visualColumn, visualRow);
    int logicalRow = d->logicalRow(visualRow);
    int logicalColumn = d->logicalColumn(visualColumn);
    if (!model()->hasIndex(logicalRow, logicalColumn, rootIndex()))
        return QModelIndex();

    QModelIndex result = model()->index(logicalRow, logicalColumn, rootIndex());
    if (!d->isRowHidden(logicalRow) && !d->isColumnHidden(logicalColumn) && d->isIndexEnabled(result))
        return result;

    return QModelIndex();
}

/*!
            \fn void QpTableView2::setSelection(const QRect &rect,
            QItemSelectionModel::SelectionFlags flags)

            Selects the items within the given \a rect and in accordance with
            the specified selection \a flags.
        */
void QpTableView2::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_D(QpTableView2);

    //if( debug_selection ) qDebug() << "QpTableView2::setSelection rect" << rect;

    QModelIndex tl = indexAt( QPoint( qMin(rect.left(), rect.right()), qMin(rect.top(), rect.bottom())));


    QModelIndex br = indexAt( QPoint( qMax(rect.left(), rect.right()), qMax(rect.top(), rect.bottom())));

    //if( debug_selection ) qDebug() << "QpTableView2::setSelection rect" << rect << "  tl "<< tl << " br " << br;


    if (!d->selectionModel || !tl.isValid() || !br.isValid() || !d->isIndexEnabled(tl) || !d->isIndexEnabled(br))
        return;

    bool verticalMoved = verticalHeader()->sectionsMoved();
    bool horizontalMoved = horizontalHeader()->sectionsMoved();

    QItemSelection selection;


    if (verticalMoved && horizontalMoved)
    {
        int top = d->visualRow(tl.row());
        int left = d->visualColumn(tl.column());
        int bottom = d->visualRow(br.row());
        int right = d->visualColumn(br.column());

        for (int horizontal = left; horizontal <= right; ++horizontal)
        {
            int column = d->logicalColumn(horizontal);

            for (int vertical = top; vertical <= bottom; ++vertical)
            {
                int row = d->logicalRow(vertical);
                QModelIndex index = model()->index(row, column, rootIndex());
                selection.append(QItemSelectionRange(index));
            }
        }
    }
    else if (horizontalMoved)
    {
        //
        int left = d->visualColumn(tl.column());
        int right = d->visualColumn(br.column());

        for (int visual = left; visual <= right; ++visual)
        {
            int column = d->logicalColumn(visual);
            QModelIndex topLeft = model()->index(tl.row(), column, rootIndex());
            QModelIndex bottomRight = model()->index(br.row(), column, rootIndex());
            selection.append(QItemSelectionRange(topLeft, bottomRight));
        }
    }
    else if (verticalMoved)
    {
        int top = d->visualRow(tl.row());
        int bottom = d->visualRow(br.row());

        for (int visual = top; visual <= bottom; ++visual)
        {
            int row = d->logicalRow(visual);
            QModelIndex topLeft = model()->index(row, tl.column(), rootIndex());
            QModelIndex bottomRight = model()->index(row, br.column(), rootIndex());
            selection.append(QItemSelectionRange(topLeft, bottomRight));
        }
    }
    else
    { // nothing moved

        //d->setHoverIndex( QModelIndex()); // !!?? lifehack

        QItemSelectionRange range(tl, br);

        if (!range.isEmpty())

            selection.append(range);
    }

    if( debug_selection ) qDebug()<< " QpTableView2::setSelection selection " << selection;

    d->selectionModel->select(selection, command);

}


QRegion QpTableView2::visualRegionForSelection(const QItemSelection &selection) const
{
    if ( debug_selection ) qDebug() << "QpTableView2::visualRegionForSelection selection: " << selection ;

    if ( debug_selection )
    {
        foreach (QItemSelectionRange itm, selection)
            if( debug_selection ) qDebug() << "   indices : - " << itm.left() << itm.right() << "   |"<< itm.top() <<itm.bottom();
    }


    Q_D(const QpTableView2);

    if (selection.isEmpty())
        return QRegion();

    QRegion selectionRegion;

    const QRect &viewportRect = viewport()->rect();
    bool verticalMoved = verticalHeader()->sectionsMoved();
    bool horizontalMoved = horizontalHeader()->sectionsMoved();

    if ((verticalMoved && horizontalMoved) )
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);
            if (range.parent() != rootIndex() || !range.isValid())
                continue;
            for (int r = range.top(); r <= range.bottom(); ++r)
                for (int c = range.left(); c <= range.right(); ++c)
                {
                    const QRect &rangeRect = visualRect(model()->index(r, c, rootIndex()));
                    if (viewportRect.intersects(rangeRect))
                        selectionRegion += rangeRect;
                }
        }
    }
    else if (horizontalMoved)
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);
            if (range.parent() != rootIndex() || !range.isValid())
                continue;
            int top = rowViewportPosition(range.top());
            int bottom = rowViewportPosition(range.bottom()) + rowHeight(range.bottom());
            if (top > bottom)
                qSwap<int>(top, bottom);
            int height = bottom - top;
            for (int c = range.left(); c <= range.right(); ++c) {
                const QRect rangeRect(columnViewportPosition(c), top, columnWidth(c), height);
                if (viewportRect.intersects(rangeRect))
                    selectionRegion += rangeRect;
            }
        }
    }
    else if (verticalMoved)
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);

            if (range.parent() != rootIndex() || !range.isValid())
                continue;

            int left = columnViewportPosition(range.left());
            int right = columnViewportPosition(range.right()) + columnWidth(range.right());

            if (left > right)
                qSwap<int>(left, right);

            int width = right - left;

            for (int r = range.top(); r <= range.bottom(); ++r)
            {
                const QRect rangeRect(left, rowViewportPosition(r), width, rowHeight(r));
                if (viewportRect.intersects(rangeRect))
                    selectionRegion += rangeRect;
            }
        }
    }
    else
    { // nothing moved

        const int gridAdjust = showGrid() ? 1 : 0;

        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);

            if( debug_selection ) qDebug()<< i << "    selection  range: " <<range;

            if (range.parent() != rootIndex() || !range.isValid())
                continue;

            d->trimHiddenSelections(&range);

            /*
                        in : a selected range by logical indices  left, top, right, bottom

                        but logical indices maybe 0,2,3,4,5 with skipping a numbers
                        and in any order

                        out: now we return each other logical index rectangle
                    */


            QModelIndex tl = selectionModel()->model()->index( 0 , range.left() );
            QModelIndex br = selectionModel()->model()->index( 0 , range.right() );

            int rH = rowHeight( range.bottom() );

            for(int ll = range.top(); ll <=range.bottom(); ll++ )
            {
                const int rtop = rowViewportPosition( ll) ;
                const int rBtm = rtop +  rH;
                int x1 = 0;
                int x2 = -1;

                for(int lgclNum = range.left(); lgclNum <=range.right(); lgclNum++ )
                {

                    if( lgclNum != range.left())
                    {
                        // remember to select labels in table, if its exist
                        //horizontalHeader()->
                    }

                    QRect  rect = columnViewportPosition2( lgclNum );

                    if( ! rect.isValid())
                    {
                        // it is the skipping number in logical numbers
                        continue;
                    }

                    int x1_ = rect.x();
                    int x2_ = x1_ + rect.width();

                    if( x1 >= 0 )
                        x1 = qMin( x1, x1_);

                    x2 = qMax( x2 , x2_);

                }

                const QRect rangeRect( QPoint(x1, rtop),
                                       QPoint(x2 -  gridAdjust + correct_width_minus_1,
                                              rBtm - gridAdjust + correct_height_minus_1));

                selectionRegion += rangeRect;

            }
        }
    }

    // resulting region

    if( debug_selection )  qDebug() << "QpTableView2::visualRegionForSelection return selectionRegion " << selectionRegion;

    return selectionRegion;
}


QModelIndexList QpTableView2::selectedIndexes() const
{
    if( debug_selection ) qDebug() << "QpTableView2::selectedIndexes()";

    Q_D(const QpTableView2);
    QModelIndexList viewSelected;
    QModelIndexList modelSelected;

    if (d->selectionModel)
        modelSelected = d->selectionModel->selectedIndexes();

    for (int i = 0; i < modelSelected.count(); ++i)
    {
        QModelIndex index = modelSelected.at(i);

        if (!isIndexHidden(index) && index.parent() == rootIndex())
            viewSelected.append(index);
    }

    return viewSelected;
}


/*!
                    This slot is called whenever rows are added or deleted. The
                    previous number of rows is specified by \a oldCount, and the new
                    number of rows is specified by \a newCount.
                */
void QpTableView2::rowCountChanged(int oldCount, int newCount )
{
    Q_D(QpTableView2);
    //when removing rows, we need to disable updates for the header until the geometries have been
    //updated and the offset has been adjusted, or we risk calling paintSection for all the sections
    if (newCount < oldCount)
        d->verticalHeader->setUpdatesEnabled(false);

    //??d->doDelayedItemsLayout();
}

/*!
                    This slot is called whenever columns are added or deleted. The
                    previous number of columns is specified by \a oldCount, and the new
                    number of columns is specified by \a newCount.
                */
void QpTableView2::columnCountChanged(int, int)
{
    Q_D(QpTableView2);
    updateGeometries();
    if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem)
        d->horizontalHeader->setOffsetToSectionPosition(horizontalScrollBar()->value());
    else
        d->horizontalHeader->setOffset(horizontalScrollBar()->value());
    viewport()->update();
}

/*!
                    \reimp
                */

void QpTableView2::updateGeometries_()
{
    updateGeometries();
}

void QpTableView2::updateGeometries()
{
    Q_D(QpTableView2);

    if (d->geometryRecursionBlock)
        return;

    d->geometryRecursionBlock = true;

    int width = 0;

    if (!d->verticalHeader->isHidden())
    {
        width = qMax(d->verticalHeader->minimumWidth(), d->verticalHeader->sizeHint().width());

        width = qMin(width, d->verticalHeader->maximumWidth());
    }

    int height = 0;

    if (!d->horizontalHeader->isHidden())
    {
        height = qMax( d->horizontalHeader->minimumHeight(), d->horizontalHeader->sizeHint().height() );

        height = qMin( height, d->horizontalHeader->maximumHeight() );
    }

    setViewportMargins(width, height, 0, 0);

    // update headers

    QRect vg = viewport()->geometry();

    int verticalLeft = vg.left() - width;

    d->verticalHeader->setGeometry(verticalLeft, vg.top(), width, vg.height());

    if (d->verticalHeader->isHidden())
        QMetaObject::invokeMethod(d->verticalHeader, "updateGeometries");

    int horizontalTop = vg.top() - height;

    d->horizontalHeader->setGeometry(vg.left(), horizontalTop, vg.width(), height);

    if (d->horizontalHeader->isHidden())
        QMetaObject::invokeMethod(d->horizontalHeader, "updateGeometries");

    // update cornerWidget
    if (d->horizontalHeader->isHidden() || d->verticalHeader->isHidden())
    {
        d->cornerWidget->setHidden(true);
    }
    else
    {
        d->cornerWidget->setHidden(false);
        d->cornerWidget->setGeometry(verticalLeft, horizontalTop, width, height);
    }

    // update scroll bars

    // ### move this block into the if
    QSize vsize = viewport()->size();
    QSize max = maximumViewportSize();
    uint horizontalLength = d->horizontalHeader->length();
    uint verticalLength = d->verticalHeader->length();

    if ((uint)max.width() >= horizontalLength && (uint)max.height() >= verticalLength)
        vsize = max;

    // --------------------------------------------------
    //          horizontal scroll bar
    // --------------------------------------------------

    //const int columnCount = d->horizontalHeader->count();
    const int xNumCount = d->horizontalHeader->count_of_section();


    int offsetX = d->horizontalHeader->offset();

    int first_xNum = d->horizontalHeader->get_section_num( offsetX );

    int vpWidth = vsize.width();

    if( debug_scroll ) qDebug() << "d->horizontalHeader->offset() : " << d->horizontalHeader->offset() << " vpWidth:" << vpWidth << " offsetX:" << offsetX;

    if( debug_scroll ) qDebug() << "    first_xNum " << first_xNum;


    int last_xNum = d->horizontalHeader->get_section_num( offsetX + vpWidth );

    if( debug_scroll ) qDebug() << "   last_xNum " << last_xNum;

    const int viewportWidth = vsize.width();

    int xNumsInViewport = 0;

    //if( debug ) qDebug() << "QpTableView2::updateGeometries";

    int width_ = 0;

    for ( int xNum = first_xNum; xNum <= last_xNum; xNum++)
    {
        //        int logical = d->horizontalHeader->logicalIndex( xNum );

        //        if( logical == -1)
        //            continue;

        //        if ( d->horizontalHeader->isSectionHidden( logical ) )
        //            continue;

        //width += d->horizontalHeader->sectionSize( logical );

        int ww = d->horizontalHeader->xNum_size( xNum );

        width_ += ww;


        if (width_ > viewportWidth)

            break;

        ++xNumsInViewport;

    }

    xNumsInViewport = qMax(xNumsInViewport, 1); //there must be always at least 1 column

    if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem)
    {
        const int visibleColumns = xNumCount - d->horizontalHeader->hiddenSectionCount();


        int rangMax = visibleColumns - xNumsInViewport;

        if( debug_scroll ) qDebug() << " QpTableView2::updateGeometries() ";
        if( debug_scroll ) qDebug() << "   visibleColumns: "<< visibleColumns;
        if( debug_scroll ) qDebug() << "   columnsInViewport: "<< xNumsInViewport;
        if( debug_scroll ) qDebug() << "   rangMax: " << rangMax;
        if( debug_scroll ) qDebug() << "   horizontalScrollBar()->value(): " << horizontalScrollBar()->value();

        horizontalScrollBar()->setRange(0, rangMax);

        horizontalScrollBar()->setPageStep( xNumsInViewport   );

        if (xNumsInViewport >= visibleColumns)
            d->horizontalHeader->setOffset(0);

        horizontalScrollBar()->setSingleStep(1);
    }
    else
    { // ScrollPerPixel
        horizontalScrollBar()->setPageStep(vsize.width());
        horizontalScrollBar()->setRange(0, horizontalLength - vsize.width());
        horizontalScrollBar()->setSingleStep(qMax(vsize.width() / (xNumsInViewport + 1), 2));
    }

    // --------------------------------------------------
    //          vertical scroll bar
    // --------------------------------------------------

    const int rowCount = d->verticalHeader->count();
    const int viewportHeight = vsize.height();
    int rowsInViewport = 0;

    for (int height = 0, row = rowCount - 1; row >= 0; --row)
    {
        int logical = d->verticalHeader->logicalIndex(row);
        if (!d->verticalHeader->isSectionHidden(logical))
        {
            height += d->verticalHeader->sectionSize(logical);

            if (height > viewportHeight)
                break;

            ++rowsInViewport;
        }
    }
    rowsInViewport = qMax(rowsInViewport, 1); //there must be always at least 1 row

    if (verticalScrollMode() == QAbstractItemView::ScrollPerItem)
    {
        const int visibleRows = rowCount - d->verticalHeader->hiddenSectionCount();

        verticalScrollBar()->setRange(0, visibleRows - rowsInViewport);

        verticalScrollBar()->setPageStep(rowsInViewport);

        if (rowsInViewport >= visibleRows)
            d->verticalHeader->setOffset(0);

        verticalScrollBar()->setSingleStep(1);
    }
    else
    { // ScrollPerPixel
        verticalScrollBar()->setPageStep(vsize.height());
        verticalScrollBar()->setRange(0, verticalLength - vsize.height());
        verticalScrollBar()->setSingleStep(qMax(vsize.height() / (rowsInViewport + 1), 2));
    }

    d->geometryRecursionBlock = false;
    QAbstractItemView::updateGeometries();
}

/*!
                    Returns the size hint for the given \a row's height or -1 if there
                    is no model.

                    If you need to set the height of a given row to a fixed value, call
                    QpHorHeaderView::resizeSection() on the table's vertical header.

                    If you reimplement this function in a subclass, note that the value you
                    return is only used when resizeRowToContents() is called. In that case,
                    if a larger row height is required by either the vertical header or
                    the item delegate, that width will be used instead.

                    \sa QWidget::sizeHint, verticalHeader()
                */
int QpTableView2::sizeHintForRow(int row) const
{
    Q_D(const QpTableView2);

    int rowH = d->horizontalHeader->row_height();

    if( rowH == -1)
        return QpTableView2::defaultRowHeight;

    return d->showGrid ? rowH + 1 : rowH;

}


int QpTableView2::sizeHintForColumn(int column) const
{
    Q_D(const QpTableView2);

    if (!model())
        return -1;

    //int hint = 100;

    ensurePolished();

    int top = qMax(0, d->verticalHeader->visualIndexAt(0));

    int bottom = d->verticalHeader->visualIndexAt(viewport()->height());

    if (!isVisible() || bottom == -1) // the table don't have enough rows to fill the viewport
        bottom = model()->rowCount(rootIndex()) - 1;

    QStyleOptionViewItemV4 option = viewOptions();//d->viewOptionsV4();

    int hint = 0;
    QModelIndex index;
    for (int row = top; row <= bottom; ++row)
    {
        int logicalRow = d->verticalHeader->logicalIndex(row);
        if (d->verticalHeader->isSectionHidden(logicalRow))
            continue;
        index = model()->index(logicalRow, column, rootIndex());

        QWidget *editor = 0;//?? d->editorForIndex(index).widget.data();

        if (editor && d->persistent.contains(editor)) {
            hint = qMax(hint, editor->sizeHint().width());
            int min = editor->minimumSize().width();
            int max = editor->maximumSize().width();
            hint = qBound(min, hint, max);
        }

        hint = qMax(hint, itemDelegate( index )->sizeHint(option, index).width());
    }

    hint = d->showGrid ? hint + 1 : hint;
    if( debug ) qDebug()<< " QpTableView2::sizeHintForColumn(column:"<<column<<")" <<  hint;
    return hint;
}

/*!
                    Returns the y-coordinate in contents coordinates of the given \a
                    row.
                */
int QpTableView2::rowViewportPosition(int row) const
{
    Q_D(const QpTableView2);

    int y = d->verticalHeader->sectionViewportPosition( row ) ;

    //if(debug) qDebug() << "QpTableView2::rowViewportPosition y:" << y;

    return y ;
}

/*!
                    Returns the row in which the given y-coordinate, \a y, in contents
                    coordinates is located.

                    \note This function returns -1 if the given coordinate is not valid
                    (has no row).

                    \sa columnAt()
                */
int QpTableView2::rowAt(int y) const
{
    Q_D(const QpTableView2);

    int row = d->verticalHeader->logicalIndexAt( y ); // QpTableView2::lines;

    //if(debug) qDebug() << "QpTableView2::rowAt(y:"<<y<<") = " << row;

    return row ;
}


void QpTableView2::setLineHeightInRow(int line, int height) const
{
   Q_D(const QpTableView2);

    horizontalHeader()->setLineHeightInRow( line , height);
}

void QpTableView2::setRowHeight(int row, int height)
{
    Q_D(const QpTableView2);
    d->verticalHeader->resizeSection(row, height);
}


int QpTableView2::rowHeight(int row) const
{
    Q_D(const QpTableView2);

    return d->verticalHeader->sectionSize(row);
}

//int QpTableView2::rowLine(int y) const
//{
//    Q_D(const QTableView2);

//    return d->horizontalHeader->sectionSize();
//}

/*!
                    Returns the x-coordinate in contents coordinates of the given \a
                    column.
                */
QRect QpTableView2::columnViewportPosition2(int logicalIndex ) const
{
    Q_D(const QpTableView2);

    QRect retSection = d->horizontalHeader->sectionViewportPosition2( logicalIndex  );


    return retSection;
}

int QpTableView2::columnViewportPosition(int column) const
{
    Q_D(const QpTableView2);

    int retSection = d->horizontalHeader->sectionViewportPosition2( column ).x();

    return retSection;
}

/*!
                    Returns the column in which the given x-coordinate, \a x, in contents
                    coordinates is located.

                    \note This function returns -1 if the given coordinate is not valid
                    (has no column).

                    \sa rowAt()
                */
int QpTableView2::columnAt( int x , int y) const
{
    // -------------------------------------------------------
    // This occures only for user actions, not for paintEvent
    // -------------------------------------------------------

    Q_D(const QpTableView2);

    // x - coordinate in viewport

    // y - coordinate in viewport

    // There is sometimes an incomplete first row
    int viewport_top = d->verticalHeader->offset();

    //int gggg= viewport()->yoffset;

    int rowH = d->horizontalHeader->row_height();

    int incomplete_first_visible_row = viewport_top %  rowH;

    if ( incomplete_first_visible_row > 0 )
    {

        int revert =  rowH - incomplete_first_visible_row;

        y -= revert;

        if( debug ) qDebug() << " incomplete_first_visible_row " << incomplete_first_visible_row << "  revert " << revert << "  y " << y;
    }



    return d->horizontalHeader->logicalIndexAt(QPoint( x , y ) ); //?????
}

/*!
                    \since 4.1

                    Sets the width of the given \a column to be \a width.
                */
void QpTableView2::setColumnWidth(int column, int width)
{
    Q_D(const QpTableView2);
    d->horizontalHeader->resizeSection(column, width);
}

int QpTableView2::columnWidth(int column) const
{
    Q_D(const QpTableView2);

    return d->horizontalHeader->sectionSize(column);
}

bool QpTableView2::isRowHidden(int row) const
{
    Q_D(const QpTableView2);
    return d->verticalHeader->isSectionHidden(row);
}

void QpTableView2::setRowHidden(int row, bool hide)
{
    Q_D(QpTableView2);
    if (row < 0 || row >= d->verticalHeader->count())
        return;
    d->verticalHeader->setSectionHidden(row, hide);
}

bool QpTableView2::isColumnHidden(int column) const
{
    Q_D(const QpTableView2);
    return d->horizontalHeader->isSectionHidden(column);
}

void QpTableView2::setColumnHidden(int column, bool hide)
{
    Q_D(QpTableView2);
    if (column < 0 || column >= d->horizontalHeader->count())
        return;
    d->horizontalHeader->setSectionHidden(column, hide);
}

/*!
                    \since 4.2
                    \property QpTableView2::sortingEnabled
                    \brief whether sorting is enabled

                    If this property is true, sorting is enabled for the table.  If
                    this property is false, sorting is not enabled. The default value
                    is false.

                    \note. Setting the property to true with setSortingEnabled()
                    immediately triggers a call to sortByColumn() with the current
                    sort section and order.

                    \sa sortByColumn()
                */

/*!
                  If \a enabled true enables sorting for the table and immediately
                  trigger a call to sortByColumn() with the current sort section and
                  order
                 */
void QpTableView2::setSortingEnabled(bool enable)
{
    Q_D(QpTableView2);
    d->sortingEnabled = enable;
    horizontalHeader()->setSortIndicatorShown(enable);
    if (enable) {
        disconnect(d->horizontalHeader, SIGNAL(sectionEntered(int)),
                   this, SLOT(_q_selectColumn(int)));
        disconnect(horizontalHeader(), SIGNAL(sectionPressed(int)),
                   this, SLOT(selectColumn(int)));
        connect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
                this, SLOT(sortByColumn(int)), Qt::UniqueConnection);
        sortByColumn(horizontalHeader()->sortIndicatorSection(),
                     horizontalHeader()->sortIndicatorOrder());
    } else {
        connect(d->horizontalHeader, SIGNAL(sectionEntered(int)),
                this, SLOT(_q_selectColumn(int)), Qt::UniqueConnection);
        connect(horizontalHeader(), SIGNAL(sectionPressed(int)),
                this, SLOT(selectColumn(int)), Qt::UniqueConnection);
        disconnect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
                   this, SLOT(sortByColumn(int)));
    }
}

bool QpTableView2::isSortingEnabled() const
{
    Q_D(const QpTableView2);
    return d->sortingEnabled;
}


bool QpTableView2::showGrid() const
{
    Q_D(const QpTableView2);

    return d->showGrid;
}

bool QpTableView2::showBetweenRowBorder() const
{
    Q_D(const QpTableView2);

    return d->showBetweenRowsBorder;

}

void QpTableView2::setShowBetweenRowBorder(bool show)
{
    Q_D(QpTableView2);

    if ( d->showBetweenRowsBorder != show)
    {
        d->showBetweenRowsBorder = show;
        viewport()->update();
    }

    //horizontalHeader()->setGridWidth( 1 );
}


void QpTableView2::setShowGrid(bool show)
{
    Q_D(QpTableView2);

    if ( d->showGrid != show)
    {
        d->showGrid = show;
        viewport()->update();
    }

    horizontalHeader()->setGridWidth( 1 );
}

/*!
                  \property QpTableView2::gridStyle
                  \brief  the pen style used to draw the grid.

                  This property holds the style used when drawing the grid (see \l{showGrid}).
                */
Qt::PenStyle QpTableView2::gridStyle() const
{
    Q_D(const QpTableView2);

    return d->gridStyle;
}

void QpTableView2::setRowGridStyle(Qt::PenStyle style)
{

    Q_D(QpTableView2);

    if (d->gridStyle != style)
    {
        d->gridStyle = style;
        viewport()->update();
    }
}

void QpTableView2::setGridStyle(Qt::PenStyle style)
{
    Q_D(QpTableView2);

    if (d->gridStyle != style)
    {
        d->gridStyle = style;
        viewport()->update();
    }
}

void QpTableView2::setWordWrap(bool on)
{
    Q_D(QpTableView2);
    if (d->wrapItemText == on)
        return;
    d->wrapItemText = on;
    QMetaObject::invokeMethod(d->verticalHeader, "resizeSections");
    QMetaObject::invokeMethod(d->horizontalHeader, "resizeSections");
}

bool QpTableView2::wordWrap() const
{
    Q_D(const QpTableView2);
    return d->wrapItemText;
}


void QpTableView2::setCornerButtonEnabled(bool enable)
{
    Q_D(QpTableView2);
    d->cornerWidget->setEnabled(enable);
}

bool QpTableView2::isCornerButtonEnabled() const
{
    Q_D(const QpTableView2);
    return d->cornerWidget->isEnabled();
}

/*!
                    \internal

                    Returns the rectangle on the viewport occupied by the given \a
                    index.
                    If the index is hidden in the view it will return a null QRect.
                */
QRect QpTableView2::visualRect(const QModelIndex &index) const
{
    // ------------------------------------------------------
    // this is called from user doubleclick on a cell (for example)
    // ------------------------------------------------------
    Q_D(const QpTableView2);

    if ( ! d->isIndexValid(index)
         || index.parent() != rootIndex()
         || isIndexHidden(index) )

        return QRect();

    ////d->executePostedLayout();

    int row_viewpport_y = rowViewportPosition(index.row());

    QRect rect = columnViewportPosition2(index.column());

    int y = rect.y();

    int offset_y = row_viewpport_y + y;

    //if( debug ) qDebug() << "QpTableView2::visualRect rect " << rect << " row_viewpport_y " << row_viewpport_y << "  offset_y " << offset_y;

    rect.moveTop( offset_y );

    //int colw = columnWidth(index.column());

    const int i = showGrid() ? 1 : 0;

    //if( debug ) qDebug() << "QpTableView2::visualRect return rect " << rect;

    return rect;
}

void QpTableView2::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_D(QpTableView2);

    if( debug_scroll ) qDebug() << "QpTableView2::scrollTo " << index << " ScrollHint hint " << hint;

    // check if we really need to do anything

    if (!d->isIndexValid(index)
            || (model()->parent(index) != rootIndex())
            || isRowHidden(index.row()) || isColumnHidden(index.column()))
        return;

    // Adjust horizontal position

    int viewportWidth = viewport()->width();
    int horizontalOffset = d->horizontalHeader->offset();

    int horizontalPosition = d->horizontalHeader->sectionPosition2(index.column()).x(); //!!

    int horizontalIndex = d->horizontalHeader->visualIndex(index.column());

    int cellWidth =  d->horizontalHeader->sectionSize(index.column());

    if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem)
    {

        bool positionAtLeft = (horizontalPosition - horizontalOffset < 0);
        bool positionAtRight = (horizontalPosition - horizontalOffset + cellWidth > viewportWidth);

        if (hint == PositionAtCenter || positionAtRight)
        {
            int w = (hint == PositionAtCenter ? viewportWidth / 2 : viewportWidth);
            int x = cellWidth;

            while (horizontalIndex > 0)
            {
                x += columnWidth( horizontalIndex-1 );
                if (x > w)
                    break;
                --horizontalIndex;
            }
        }

        if (positionAtRight || hint == PositionAtCenter || positionAtLeft)
        {
            int hiddenSections = 0;
            if (d->horizontalHeader->sectionsHidden())
            {
                for (int s = horizontalIndex - 1; s >= 0; --s)
                {

                    int column = s ;

                    if (d->horizontalHeader->isSectionHidden(column))
                        ++hiddenSections;
                }
            }
            horizontalScrollBar()->setValue(horizontalIndex - hiddenSections);
        }

    }
    else { // ScrollPerPixel
        if (hint == PositionAtCenter)
        {
            horizontalScrollBar()->setValue(horizontalPosition - ((viewportWidth - cellWidth) / 2));
        }
        else
        {
            if (horizontalPosition - horizontalOffset < 0 || cellWidth > viewportWidth)
                horizontalScrollBar()->setValue(horizontalPosition);
            else if (horizontalPosition - horizontalOffset + cellWidth > viewportWidth)
                horizontalScrollBar()->setValue(horizontalPosition - viewportWidth + cellWidth);
        }
    }

    // Adjust vertical position

    int viewportHeight = viewport()->height();
    int verticalOffset = d->verticalHeader->offset();
    int verticalPosition = d->verticalHeader->sectionPosition(index.row());
    int verticalIndex = d->verticalHeader->visualIndex(index.row());
    //    int cellHeight = d->hasSpans()
    //            ? d->rowSpanHeight(index.row(), span.height())
    //            : d->verticalHeader->sectionSize(index.row());
    int cellHeight =  d->verticalHeader->sectionSize(index.row());

    if (verticalPosition - verticalOffset < 0 || cellHeight > viewportHeight)
    {
        if (hint == EnsureVisible)
            hint = PositionAtTop;
    }
    else if (verticalPosition - verticalOffset + cellHeight > viewportHeight)
    {
        if (hint == EnsureVisible)
            hint = PositionAtBottom;
    }

    if (verticalScrollMode() == QAbstractItemView::ScrollPerItem)
    {

        if (hint == PositionAtBottom || hint == PositionAtCenter)
        {
            int h = (hint == PositionAtCenter ? viewportHeight / 2 : viewportHeight);
            int y = cellHeight;
            while (verticalIndex > 0)
            {
                int row = d->verticalHeader->logicalIndex(verticalIndex - 1);
                y += d->verticalHeader->sectionSize(row);
                if (y > h)
                    break;
                --verticalIndex;
            }
        }

        if (hint == PositionAtBottom || hint == PositionAtCenter || hint == PositionAtTop)
        {
            int hiddenSections = 0;
            if (d->verticalHeader->sectionsHidden())
            {
                for (int s = verticalIndex - 1; s >= 0; --s)
                {
                    int row = d->verticalHeader->logicalIndex(s);
                    if (d->verticalHeader->isSectionHidden(row))
                        ++hiddenSections;
                }
            }
            verticalScrollBar()->setValue(verticalIndex - hiddenSections);
        }

    } else { // ScrollPerPixel
        if (hint == PositionAtTop)
        {
            verticalScrollBar()->setValue(verticalPosition);
        }
        else if (hint == PositionAtBottom)
        {
            verticalScrollBar()->setValue(verticalPosition - viewportHeight + cellHeight);
        }
        else if (hint == PositionAtCenter)
        {
            verticalScrollBar()->setValue(verticalPosition - ((viewportHeight - cellHeight) / 2));
        }
    }

    update(index);
}

/*!
                    This slot is called to change the height of the given \a row. The
                    old height is specified by \a oldHeight, and the new height by \a
                    newHeight.

                    \sa columnResized()
                */
void QpTableView2::rowResized(int row, int oldHeight, int newHeight)
{
    Q_D(QpTableView2);
    d->rowsToUpdate.append(row);

    if (d->rowResizeTimerID == 0)
        d->rowResizeTimerID = startTimer(0);
}

void QpTableView2::columnResized_Y()
{
    Q_D(QpTableView2);

    int hh = horizontalHeader()->row_height();

    verticalHeader()->setDefaultSectionSize( hh );

    //    QRect hREct = horizontalHeader()->rect();

    //    repaint( viewport()->rect() );
    int firstVisualRow = qMax(verticalHeader()->visualIndexAt( 0 ) , 0);

    int lastVisualRow = verticalHeader()->visualIndexAt(verticalHeader()->viewport()->height() );

    for( int row = firstVisualRow; row<=lastVisualRow; row++ )
        d->rowsToUpdate.append(row);

    if (d->rowResizeTimerID == 0)
        d->rowResizeTimerID = startTimer(0);

    //    d->columnsToUpdate.append(column);

    //    if (d->columnResizeTimerID == 0)
    //        d->columnResizeTimerID = startTimer(0);
}

void QpTableView2::columnResized(int column, int oldWidth, int newWidth)
{
    Q_D(QpTableView2);

    //d->horizontalHeader->resizeSection( column );

    d->columnsToUpdate.append(column);

    if (d->columnResizeTimerID == 0)
        d->columnResizeTimerID = startTimer(0);
}


void QpTableView2::timerEvent(QTimerEvent *event)
{
    Q_D(QpTableView2);

    // -----------------------------------------------------------------------
    // This timer is only for repainting with mouse changes section sizes
    // -----------------------------------------------------------------------

    if (event->timerId() == d->columnResizeTimerID)
    {
        if( debug_event ) qDebug() << "QpTableView2::timerEvent columnResizeTimerID";

        updateGeometries();

        killTimer(d->columnResizeTimerID);

        d->columnResizeTimerID = 0;

        QRect rect;
        int viewportHeight = viewport()->height();
        int viewportWidth = viewport()->width();


        for (int i = d->columnsToUpdate.size()-1; i >= 0; --i)
        {
            int column = d->columnsToUpdate.at(i);

            int x = horizontalHeader()->left_common_border_x( column) ;

            rect |= QRect(x, 0, viewportWidth - x, viewportHeight);
        }


        viewport()->update(rect.normalized());
        d->columnsToUpdate.clear();
    }

    if (event->timerId() == d->rowResizeTimerID)
    {
        if( debug_event ) qDebug() << "QpTableView2::timerEvent rowResizeTimerID";

        updateGeometries();
        killTimer(d->rowResizeTimerID);
        d->rowResizeTimerID = 0;

        int viewportHeight = viewport()->height();
        int viewportWidth = viewport()->width();
        int top;

        top = viewportHeight;
        for (int i = d->rowsToUpdate.size()-1; i >= 0; --i)
        {
            int y = rowViewportPosition(d->rowsToUpdate.at(i));
            top = qMin(top, y);
        }

        viewport()->update(QRect(0, top, viewportWidth, viewportHeight - top));

        d->rowsToUpdate.clear();
    }

    if (event->timerId() == delayed_Repaint_tmr)
    {
        if( debug_selection | debug_event ) qDebug() << "QpTableView2::timerEvent delayed_Repaint_tmr";

        killTimer( delayed_Repaint_tmr);
        delayed_Repaint_tmr = 0;

        d->horizontalHeader->viewport()->updateGeometry();
        d->horizontalHeader->viewport()->update();

        d->verticalHeader->viewport()->updateGeometry();
        d->verticalHeader->viewport()->update();

        //clearSelection();
        //updateGeometries();
        viewport()->update();

    }

    QAbstractItemView::timerEvent(event);
}

/*!
                    This slot is called to change the index of the given \a row in the
                    table view. The old index is specified by \a oldIndex, and the new
                    index by \a newIndex.

                    \sa columnMoved()
                */
void QpTableView2::rowMoved(int, int oldIndex, int newIndex)
{
    Q_D(QpTableView2);

    updateGeometries();
    int logicalOldIndex = d->verticalHeader->logicalIndex(oldIndex);
    int logicalNewIndex = d->verticalHeader->logicalIndex(newIndex);

    int oldTop = rowViewportPosition(logicalOldIndex);
    int newTop = rowViewportPosition(logicalNewIndex);
    int oldBottom = oldTop + rowHeight(logicalOldIndex);
    int newBottom = newTop + rowHeight(logicalNewIndex);
    int top = qMin(oldTop, newTop);
    int bottom = qMax(oldBottom, newBottom);
    int height = bottom - top;
    viewport()->update(0, top, viewport()->width(), height); //!!
}

/*!
                    This slot is called to change the index of the given \a column in
                    the table view. The old index is specified by \a oldIndex, and
                    the new index by \a newIndex.

                    \sa rowMoved()
                */
void QpTableView2::columnMoved(int, int oldIndex, int newIndex)
{
    Q_D(QpTableView2);

    updateGeometries();
    int logicalOldIndex = d->horizontalHeader->logicalIndex(oldIndex);
    int logicalNewIndex = d->horizontalHeader->logicalIndex(newIndex);

    int oldLeft = columnViewportPosition(logicalOldIndex);
    int newLeft = columnViewportPosition(logicalNewIndex);
    int oldRight = oldLeft + columnWidth(logicalOldIndex);
    int newRight = newLeft + columnWidth(logicalNewIndex);
    int left = qMin(oldLeft, newLeft);
    int right = qMax(oldRight, newRight);
    int width = right - left;
    viewport()->update(left, 0, width, viewport()->height()); //!!
}

void QpTableView2::selectRow(int row)
{
    Q_D(QpTableView2);

    //horizontalHeader()->clearSelection(); //!!
    d->selectRow(row, true);
}

void QpTableView2::selectColumn(int column)
{
    Q_D(QpTableView2);
    d->selectColumn(column, true);
}

void QpTableView2::hideRow(int row)
{
    Q_D(QpTableView2);
    d->verticalHeader->hideSection(row);
}

void QpTableView2::hideColumn(int column)
{
    Q_D(QpTableView2);
    d->horizontalHeader->hideSection(column);
}

void QpTableView2::showRow(int row)
{
    Q_D(QpTableView2);
    d->verticalHeader->showSection(row);
}

void QpTableView2::showColumn(int column)
{
    Q_D(QpTableView2);
    d->horizontalHeader->showSection(column);
}

void QpTableView2::resizeRowToContents(int row)
{
    Q_D(QpTableView2);

    int content = sizeHintForRow(row);

    int header = d->verticalHeader->sectionSizeHint(row);

    d->verticalHeader->resizeSection(row, qMax(content, header));
}

void QpTableView2::resizeRowsToContents()
{
    Q_D(QpTableView2);

    d->verticalHeader->resizeSections( QpVertHeaderView::ResizeToContents );
}

void QpTableView2::resizeColumnToContents(int column)
{
    Q_D(QpTableView2);

    int content = sizeHintForColumn(column);

    int header = d->horizontalHeader->sectionSizeHint(column);

    d->horizontalHeader->resizeSection(column, qMax(content, header));
}

/*!
                    Resizes all columns based on the size hints of the delegate
                    used to render each item in the columns.
                */
void QpTableView2::resizeColumnsToContents()
{
    Q_D(QpTableView2);
    d->horizontalHeader->resizeSections(QpHorHeaderView::ResizeToContents);
}

/*!
                  \obsolete
                  \overload

                  Sorts the model by the values in the given \a column.
                */
void QpTableView2::sortByColumn(int column)
{
    Q_D(QpTableView2);
    if (column == -1)
        return;
    model()->sort(column, d->horizontalHeader->sortIndicatorOrder());
}

/*!
                  \since 4.2

                  Sorts the model by the values in the given \a column in the given \a order.

                  \sa sortingEnabled
                 */
void QpTableView2::sortByColumn(int column, Qt::SortOrder order)
{
    Q_D(QpTableView2);
    d->horizontalHeader->setSortIndicator(column, order);
    sortByColumn(column);
}

/*!
                    \internal
                */
void QpTableView2::verticalScrollbarAction(int action)
{
    if( debug_scroll) qDebug() << "QpTableView2::verticalScrollbarAction";

    QAbstractItemView::verticalScrollbarAction(action); // NOTHING
}

/*!
                    \internal
                */
void QpTableView2::horizontalScrollbarAction(int action)
{
    if( debug_scroll) qDebug() << "QpTableView2::horizontalScrollbarAction " << action;

    QAbstractItemView::horizontalScrollbarAction(action);
}

/*!
                  \reimp
                */
bool QpTableView2::isIndexHidden(const QModelIndex &index) const
{
    Q_D(const QpTableView2);
    Q_ASSERT(d->isIndexValid(index));
    if (isRowHidden(index.row()) || isColumnHidden(index.column()))
        return true;
    //    if (d->hasSpans()) {
    //        QSpanCollection2::Span span = d->span(index.row(), index.column());
    //        return !((span.top() == index.row()) && (span.left() == index.column()));
    //    }
    return false;
}


void QpTableView2Private::_q_selectRow(int row)
{
    selectRow(row, false);
}

void QpTableView2Private::_q_selectColumn(int column)
{
    selectColumn(column, false);
}

void QpTableView2Private::selectRow(int row, bool anchor)
{
    if( debug_selection ) qDebug() << "QpTableView2Private::selectRow row:" << row;

    Q_Q(QpTableView2);

    if (q->selectionBehavior() == QpTableView2::SelectColumns
            || (q->selectionMode() == QpTableView2::SingleSelection
                && q->selectionBehavior() == QpTableView2::SelectItems))
        return;

    if( model->columnCount() <=0)

        return;

    if (row >= 0 && row < model->rowCount(root))
    {
        // horizontalHeader->logicalIndexAt( QPoint( 0 , 0 ) ); //??????

        int column = 0; // !!!!! column 0 has to be forever

        QModelIndex index = model->index(row, column, root);

        QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);

        qDebug() << "QpTableView2Private::selectRow command:" << command;

        selectionModel->setCurrentIndex(index, QItemSelectionModel::NoUpdate);

        if ((anchor && !(command & QItemSelectionModel::Current))
                || (q->selectionMode() == QpTableView2::SingleSelection))

            rowSectionAnchor = row;

        if (q->selectionMode() != QpTableView2::SingleSelection
                && command.testFlag(QItemSelectionModel::Toggle))
        {
            if (anchor)
                ctrlDragSelectionFlag = verticalHeader->selectionModel()->selectedRows().contains(index)
                        ? QItemSelectionModel::Deselect : QItemSelectionModel::Select;

            command &= ~QItemSelectionModel::Toggle;
            command |= ctrlDragSelectionFlag;

            if (!anchor)
                command |= QItemSelectionModel::Current;
        }

        int firstLgclIdx = 0;// !! only from 0 . This is important for row selection

        QModelIndex tl = model->index( row , firstLgclIdx , root);

        if(! tl.isValid())
            qDebug() <<"fasddsafdsfdsfd";

        int lastLgclIdx = horizontalHeader->lastLogicalNum();

        QModelIndex br = model->index(qMax(rowSectionAnchor, row), lastLgclIdx , root); //!!

        if(! br.isValid())
            qDebug() <<"fasddsafdsfdsfd";

        if (verticalHeader->sectionsMoved() && tl.row() != br.row())
            q->setSelection(q->visualRect(tl)|q->visualRect(br), command);
        else
            selectionModel->select(QItemSelection(tl, br), command);
    }
}


bool QpTableView2::is_RowSelected(int row)
{
    Q_D(QpTableView2);

    qDebug() << "QpTableView2Private::is_RowSelected row:" << row << " rowSectionAnchor:" << d->rowSectionAnchor ;

    if ( row == d->rowSectionAnchor)
        return true;

    return false;
}

void QpTableView2Private::selectColumn(int column, bool anchor)

{
    if( debug_selection ) qDebug() << "QpTableView2Private::selectColumn column:" << column;

    Q_Q(QpTableView2);

    if (q->selectionBehavior() == QpTableView2::SelectRows
            || (q->selectionMode() == QpTableView2::SingleSelection
                && q->selectionBehavior() == QpTableView2::SelectItems))

        return;

    if (column >= 0 && column < model->columnCount(root))
    {
        int row = verticalHeader->logicalIndexAt( 0 );

        QModelIndex index = model->index(row, column, root);

        QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);

        selectionModel->setCurrentIndex(index, QItemSelectionModel::NoUpdate);

        if ((anchor && !(command & QItemSelectionModel::Current))
                || (q->selectionMode() == QpTableView2::SingleSelection))
            columnSectionAnchor = column;

        if (q->selectionMode() != QpTableView2::SingleSelection
                && command.testFlag(QItemSelectionModel::Toggle))
        {
            if (anchor)
                ctrlDragSelectionFlag = horizontalHeader->selectionModel()->selectedColumns().contains(index)
                        ? QItemSelectionModel::Deselect : QItemSelectionModel::Select;

            command &= ~QItemSelectionModel::Toggle;
            command |= ctrlDragSelectionFlag;

            if (!anchor)
                command |= QItemSelectionModel::Current;
        }

        QModelIndex tl = model->index(0, qMin(columnSectionAnchor, column), root);

        QModelIndex br = model->index(model->rowCount(root) - 1,
                                      qMax(columnSectionAnchor, column), root);

        if (horizontalHeader->sectionsMoved() && tl.column() != br.column())
            q->setSelection(q->visualRect(tl)|q->visualRect(br), command);
        else
            selectionModel->select(QItemSelection(tl, br), command);
    }
}


void QpTableView2::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    //qDebug() << "QpTableView2::currentChanged not debugging current "<< current << "  previous "<<previous;

#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive())
    {
        if (current.isValid())
        {
#ifdef Q_WS_X11
            Q_D(QTableView2);
            int entry = d->accessibleTable2Index(current);
            QAccessible::updateAccessibility(this, entry, QAccessible::Focus);
#else
            int entry = visualIndex(current) + 1;
            if (horizontalHeader())
                ++entry;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::Focus);
#endif
        }
    }
#endif
    QAbstractItemView::currentChanged(current, previous);
}


void QpTableView2::selectionChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
{
    if ( debug_selection )
    {
        qDebug() << "QpTableView2::selectionChanged selected:" << selected;
        qDebug() <<  " deselected:"<<deselected;
    }
    Q_D(QpTableView2);

#ifndef QT_NO_ACCESSIBILITY

    if (QAccessible::isActive())
    {
        // ### does not work properly for selection ranges.
        QModelIndex sel = selected.indexes().value(0);

        if (sel.isValid())
        {
#ifdef Q_WS_X11
            int entry = d->accessibleTable2Index(sel);
            QAccessible::updateAccessibility(this, entry, QAccessible::Selection);
#else
            int entry = visualIndex(sel);
            if (horizontalHeader())
                ++entry;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::Selection);
#endif
        }

        QModelIndex desel = deselected.indexes().value(0);

        if (desel.isValid())
        {
#ifdef Q_WS_X11
            int entry = d->accessibleTable2Index(desel);
            QAccessible::updateAccessibility(this, entry, QAccessible::SelectionRemove);
#else
            int entry = visualIndex(sel);
            if (horizontalHeader())
                ++entry;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::SelectionRemove);
#endif
        }
    }
#endif

    QAbstractItemView::selectionChanged(selected, deselected);
}

//void QpTableView2::update()
//{
//    qDebug() << "QpTableView2::update";

//    QWidget::update();
//}

//void QpTableView2::update(const QRect &rect)
//{
//    qDebug() << "QpTableView2::update(rect" << rect <<")";

//    QWidget::update( rect );
//}

void QpTableView2::updateGeometry()
{
    qDebug() << "QpTableView2::updateGeometry";

    QWidget::updateGeometry();
}

int QpTableView2::visualIndex(const QModelIndex &index) const
{
    return index.row();
}

bool QpTableView2::init_template( const Qp_SECTION_TMPL &matrix )
{
    //reset();


    horizontalScrollBar()->setValue( 0 ); //!!
    verticalScrollBar()->setValue( 0 ); //!!

    //selectRow( 0 );

    if( horizontalHeader()->init_sections_template( model(), matrix ) )
    {
        doItemsLayout();

        updateGeometry(); //!!

        //repaint();

        delayed_Repaint();//!!

        clearSelection();

        return true;
    }

    return false;
}

void QpTableView2::slot_rowsInserted(const QModelIndex &parent, int first, int last)
{
    if( debug ) qDebug() << "QpTableView2::slot_rowsInserted first " << first << " last " <<last;
    //qDebug() << "    rowHeight   " << rowHeight( );

}

void QpTableView2::delayed_Repaint()
{
    if (delayed_Repaint_tmr == 0)
        delayed_Repaint_tmr = startTimer(0);

}

QT_END_NAMESPACE

#include "qp_tableview2.moc"

#include "moc_qp_tableview2.cpp"

#endif // QT_NO_TABLEVIEW
