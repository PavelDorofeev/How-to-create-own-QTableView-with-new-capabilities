/******************************************************************************
**
** Contact: BIT Ltd Company (p@kkmspb.ru) Individual Taxpayer Number (ITN Russia) 7826152874
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
#include "qp_tableview.h"

#ifndef QT_NO_TABLEVIEW

#include "qp/tableview/qp_horheaderview.h"
#include "qp/tableview/qp_vertheaderview.h"
#include "qp/tableview/qp_tableview_p.h"
#include "qp/tableview/qp_common.h"

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
#include <QSqlTableModel>
//#include "pbltableview/pblsqlrelationaltablemodel.h"

#ifndef QT_NO_ACCESSIBILITY
#include <qaccessible.h>
#endif

QT_BEGIN_NAMESPACE

const int QpTableView::defaultRowHeight = 30;

const bool QpTableView::debug = false;
const bool QpTableView::debug_init = true;
const bool QpTableView::debug_paint = false;
const bool QpTableView::debug_paint_row_col = false;
const bool QpTableView::debug_geometry = false;
const bool QpTableView::debug_event = false;
const bool QpTableView::debug_paint_region = false;
const bool QpTableView::debug_paint_border = false;
const bool QpTableView::debug_resize = false;
const bool QpTableView::debug_mdl_signals = true;
const bool QpTableView::debug_selection = false;
const bool QpTableView::debug_scroll = true;

const bool QpTableViewPrivate::debug_mdl_signals = true;
const bool QpTableViewPrivate::debug = false;
const bool QpTableViewPrivate::debug_selection = false;
const bool QpTableViewPrivate::debug_init = true;

const int QpTableView::correct_width_minus_1 = -1;
const int QpTableView::correct_height_minus_1 = -1;

const QString QpTableView::mdlIsEmptyStr = "model is empty yet (use method setTable first)";

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

void QpTableViewPrivate::init( const qp::SECTIONS_TMPL matrix )
{
    Q_Q(QpTableView);

    if ( debug_init ) qDebug() << "QpTableViewPrivate::init()" ;

    q->setEditTriggers( editTriggers |QpAbstractItemView::AnyKeyPressed);

    QpHorHeaderView *horizontal = new QpHorHeaderView( Qt::Horizontal, q);

    horizontal->setClickable(true);
    horizontal->setHighlightSections(true);

    qDebug() << "QpTableViewPrivate::init " << qp::print_matrix( matrix );

    q->setHorizontalHeader( horizontal , matrix );

    QpVertHeaderView *vertical = new QpVertHeaderView( *horizontal, Qt::Vertical, q);
    vertical->setClickable(true);
    vertical->setHighlightSections(true);

    q->setVerticalHeader(vertical);


    tabKeyNavigation = true;

    cornerWidget = new QTableCornerButton2(q);
    cornerWidget->setFocusPolicy(Qt::NoFocus);

    Q_ASSERT ( QObject::connect(cornerWidget, SIGNAL(clicked()), q, SLOT(selectAll())) == true);


}

/*!
  \internal
  Trims away indices that are hidden in the treeview due to hidden horizontal or vertical sections.
*/

void QpTableViewPrivate::trimHiddenSelections(QItemSelectionRange *range) const
{
    //if ( debug_selection ) qDebug() << "QpTableViewPrivate::trimHiddenSelections";

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

void QpTableViewPrivate::drawCellLabel(QPainter *painter,
                                       const QStyleOptionViewItemV4 &option,
                                       //const QString &txt,
                                       bool rowSelected,
                                       int row )
{
    Q_Q(QpTableView);

    QStyleOptionViewItemV4 opt = option;

    //opt.displayAlignment = stl.align;

    // -------------------------------------------------------------
    // lifehack: label selection is detected by index (row,0)
    // -------------------------------------------------------------

    QModelIndex index = model->index( row, 0 , root);


    if(  selectionModel && q->selectionBehavior() == QpAbstractItemView::SelectRows  &&  index.isValid() )
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

    //opt.text = txt;


    //QStyleOptionViewItemV4 opt = option;
    //initStyleOption(&opt, index);

    if (const QStyleOptionViewItemV4 *v4 = qstyleoption_cast<const QStyleOptionViewItemV4 *>(&opt))
    {

        const QWidget *widget = v4->widget;

        QStyle *style = widget ? widget->style() : QApplication::style();

        //QModelIndex index = model->index( row, 0);

        //QAbstractItemDelegate * del = q->itemDelegate(index);//->paint(painter, opt, index); // this paints text

        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

        //        if ( debug_selection )qDebug() <<"                    QpTableViewPrivate::drawCellLabel  drawControl row:"<<row<< " opt.text:"<< opt.text << " rowSelected "<< rowSelected;
        //        if ( debug_selection )qDebug() <<"                        opt.state " << opt.state;
        //        if ( debug_selection )qDebug() <<"                        widget " << widget;
    }
    else
        if ( debug_selection )qDebug()<<"sdafdsfasdfwe4rere";

}

void QpTableViewPrivate::drawCell(QPainter *painter, const QStyleOptionViewItemV4 &option, const QModelIndex &index)
{
    Q_Q(QpTableView);

    QStyleOptionViewItemV4 opt = option;

    if (selectionModel && selectionModel->isSelected(index))
    {
        opt.state |= QStyle::State_Selected;
    }

    if (index == hover)
    {
        opt.state |= QStyle::State_MouseOver;
    }

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


    // ---------------------------------------------------------
    //  new styles algorithm
    // ---------------------------------------------------------

    bool painterChanged = false;

    int col = index.column();

    if( stls.contains( col ) )
    {
        painter->save();

        painterChanged = true;

        QFontMetrics fntMtr = painter->fontMetrics();

        correct_Style( stls[ col ], opt);

        painter->setFont( opt.font ); // this is set font size

    }

    if (index == q->currentIndex())
    {
        const bool focus = (q->hasFocus() || viewport->hasFocus()) && q->currentIndex().isValid();

        if (focus)
            opt.state |= QStyle::State_HasFocus;
    }

    q->itemDelegate(index)->paint( painter, opt, index ); // this paints text

    if ( painterChanged )
        painter->restore();

}


QpTableView::QpTableView( const qp::SECTIONS_TMPL matrix, QWidget *parent)
    :
      QpAbstractItemView(*new QpTableViewPrivate, parent),
      delayed_Repaint_tmr(0),
      row_selected(-1)
{
    Q_D(QpTableView);

    d->init( matrix );


    Q_ASSERT ( connect(this, SIGNAL( clicked(QModelIndex) ),
                       this , SLOT( slot_clicked(QModelIndex)) ) == true);

    //viewport()->setBackgroundRole( QPalette::Button );
    //setBackgroundRole( QPalette::Button ); //!!
    if(debug) qDebug() << "ctor QpTableView::QpTableView(QWidget *parent): QpAbstractItemView(*new QpTableViewPrivate, parent) backgroundRole:" <<  backgroundRole();

}

QpTableView::QpTableView(QpTableViewPrivate &dd,  const qp::SECTIONS_TMPL matrix, QWidget *parent)
    : QpAbstractItemView(dd, parent),
      row_selected(-1)
{
    Q_D(QpTableView);


    d->init( matrix );

    if( debug ) qDebug() << "ctor QpTableView (QpTableViewPrivate&, QWidget *) backgroundRole:" << backgroundRole();
    //viewport()->setBackgroundRole( QPalette::Button );
    //setBackgroundRole( QPalette::Button ); //!!

}


QpTableView::~QpTableView()
{
}


void QpTableView::setModel (QAbstractItemModel *model )
{
    Q_D(QpTableView);


    if (model == d->model)
        return;


    //let's disconnect from the old model
    if (d->model && d->model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        Q_ASSERT ( disconnect(d->model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this, SLOT(_q_updateSpanInsertedRows(QModelIndex,int,int))) == true);

        Q_ASSERT ( disconnect(d->model, SIGNAL(columnsInserted(QModelIndex,int,int)),
                   this, SLOT(_q_updateSpanInsertedColumns(QModelIndex,int,int))) == true);

        Q_ASSERT ( disconnect(d->model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                   this, SLOT(_q_updateSpanRemovedRows(QModelIndex,int,int))) == true);

        Q_ASSERT ( disconnect(d->model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
                   this, SLOT(_q_updateSpanRemovedColumns(QModelIndex,int,int))) == true);
    }
    if (model)
    { //and connect to the new one
        qDebug() << "QpTableView::setModel";

        Q_ASSERT ( connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(_q_updateSpanInsertedRows(QModelIndex,int,int))) == true);

        Q_ASSERT ( connect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),
                this, SLOT(_q_updateSpanInsertedColumns(QModelIndex,int,int))) == true);

        Q_ASSERT ( connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(_q_updateSpanRemovedRows(QModelIndex,int,int))) == true);

        Q_ASSERT ( connect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
                this, SLOT(_q_updateSpanRemovedColumns(QModelIndex,int,int))) == true);
    }

    d->horizontalHeader->setModel( model ); // !!


    d->verticalHeader->setModel( model );

    QpAbstractItemView::setModel(model);
}




void QpTableView::setRootIndex(const QModelIndex &index)
{
    Q_D(QpTableView);

    if (index == d->root)
    {
        viewport()->update();
        return;
    }
    d->verticalHeader->setRootIndex(index);

    d->horizontalHeader->setRootIndex(index);

    QpAbstractItemView::setRootIndex(index);
}

void QpTableView::doItemsLayout()
{
    Q_D(QpTableView);

    QpAbstractItemView::doItemsLayout();

    if( debug_scroll) qDebug() << "QpTableView::doItemsLayout() verticalScrollBar()->value()" << verticalScrollBar()->value();

    if (verticalScrollMode() == QpAbstractItemView::ScrollPerItem)

        d->verticalHeader->setOffsetToSectionPosition(verticalScrollBar()->value());

    else

        d->verticalHeader->setOffset(verticalScrollBar()->value());


    if (!d->verticalHeader->updatesEnabled())

        d->verticalHeader->setUpdatesEnabled(true);
}

void QpTableView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    if ( debug_init ) qDebug() << "QpTableView::setSelectionModel "<<selectionModel;

    Q_D(QpTableView);

    Q_ASSERT(selectionModel);

    d->verticalHeader->setSelectionModel(selectionModel);

    d->horizontalHeader->setSelectionModel(selectionModel);

    QpAbstractItemView::setSelectionModel(selectionModel);
}


QpHorHeaderView *QpTableView::horizontalHeader() const
{
    Q_D(const QpTableView);
    return d->horizontalHeader;
}


QpVertHeaderView *QpTableView::verticalHeader() const
{
    Q_D(const QpTableView);

    return d->verticalHeader;
}


void QpTableView::setHorizontalHeader(QpHorHeaderView *header ,
                                      const qp::SECTIONS_TMPL matrix)
{
    Q_D(QpTableView);

    qDebug() << "QpTableView::setHorizontalHeader";

    if ( !header || header == d->horizontalHeader)
        return;

    if (d->horizontalHeader && d->horizontalHeader->parent() == this)
        delete d->horizontalHeader;

    d->horizontalHeader = header;
    d->horizontalHeader->setParent(this);

    if ( d->model && ! d->horizontalHeader->model() )
    {

        //        if ( d->model != QAbstractItemModelPrivate::staticEmptyModel() )
        //            ;

        d->horizontalHeader->setModel( d->model );

        qDebug() << "QpTableView::setHorizontalHeader: " <<qp::print_matrix( matrix );

        init_template( matrix );

        //d->horizontalHeader->set_matrix(  );


        if (d->selectionModel)
            d->horizontalHeader->setSelectionModel(d->selectionModel);
    }

    Q_ASSERT (connect(d->horizontalHeader,SIGNAL(sectionResized_Y()),
                      this, SLOT(linesInRowResized_Y())) == true); //!!

    Q_ASSERT (connect(d->horizontalHeader,SIGNAL(sectionResized_X(int)),
                      this, SLOT(xNumsResized_X(int)))== true); //!!;

    //    Q_ASSERT (connect(d->horizontalHeader,SIGNAL(sectionResized(int,int,int)),
    //            this, SLOT(columnResized(int,int,int))) == true);

    Q_ASSERT (connect(d->horizontalHeader, SIGNAL(sectionMoved(int,int,int)),
                      this, SLOT(columnMoved(int,int,int)))== true);

    Q_ASSERT (connect(d->horizontalHeader, SIGNAL(sectionCountChanged(int,int)),
                      this, SLOT(columnCountChanged(int,int)))== true);

    Q_ASSERT (connect(d->horizontalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectColumn(int)))== true);

    Q_ASSERT (connect(d->horizontalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectColumn(int)))== true);

    Q_ASSERT (connect(d->horizontalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
                      this, SLOT(resizeColumnToContents(int)))== true);

    Q_ASSERT (connect(d->horizontalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()))== true);

    //update the sorting enabled states on the new header
    setSortingEnabled(d->sortingEnabled);
}


void QpTableView::slot_clicked(const QModelIndex & idx)
{
    if( selectionBehavior() ==  SelectRows )
        if ( idx.isValid())
        {
            selectRow( idx.row() );
        }

}

void QpTableView::setVerticalHeader(QpVertHeaderView *header)
{

    Q_D(QpTableView);

    if (!header || header == d->verticalHeader)
        return;

    if (d->verticalHeader && d->verticalHeader->parent() == this)
        delete d->verticalHeader;

    d->verticalHeader = header;
    d->verticalHeader->setParent(this);

    if (!d->verticalHeader->model())
    {
        d->verticalHeader->setModel( d->model ) ;

        if (d->selectionModel)
            d->verticalHeader->setSelectionModel(d->selectionModel);
    }

    Q_ASSERT (connect(d->verticalHeader, SIGNAL(sectionResized(int,int,int)),
                      this, SLOT(rowResized(int,int,int)))== true );

    Q_ASSERT (connect(d->verticalHeader, SIGNAL(sectionMoved(int,int,int)),
                      this, SLOT(rowMoved(int,int,int)))== true );

    Q_ASSERT (connect(d->verticalHeader, SIGNAL(sectionCountChanged(int,int)),
                      this, SLOT(rowCountChanged(int,int)))== true );

    Q_ASSERT (connect(d->verticalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectRow(int))) == true );

    Q_ASSERT ( connect(d->verticalHeader, SIGNAL(sectionEntered(int)), this, SLOT( _q_selectRow(int) )) == true );

    Q_ASSERT (connect(d->verticalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
                      this, SLOT(resizeRowToContents(int))) == true );

    Q_ASSERT (connect(d->verticalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries())) == true );
}


void QpTableView::scrollContentsBy(int dx, int dy)
{
    Q_D(QpTableView);

    // dx это шаг в направлении - вправо, + влево

    if( debug_scroll ) qDebug() << "QpTableView::scrollContentsBy dx " << dx << " dy " <<dy;

    d->delayedAutoScroll.stop(); // auto scroll was canceled by the user scrolling

    //dx = isRightToLeft() ? -dx : dx;

    if ( dx )
    {
        if ( horizontalScrollMode() == QpAbstractItemView::ScrollPerItem )
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
        if (verticalScrollMode() == QpAbstractItemView::ScrollPerItem)
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

    d->scrollContentsBy(dx, dy);

    if (d->showGrid)
    {
        //we need to update the first line of the previous top item in the view
        //because it has the grid drawn if the header is invisible.
        //It is strictly related to what's done at then end of the paintEvent
        if (dy > 0 && d->horizontalHeader->isHidden() && d->verticalScrollMode == ScrollPerItem)
        {
            d->viewport->update(0, dy, d->viewport->width(), dy);
        }
        if (dx > 0 && d->verticalHeader->isHidden() && d->horizontalScrollMode == ScrollPerItem)
        {
            d->viewport->update(dx, 0, dx, d->viewport->height());
        }
    }
}

QStyleOptionViewItem QpTableView::viewOptions() const
{
    QStyleOptionViewItem option = QpAbstractItemView::viewOptions();

    option.showDecorationSelected = true;

    return option;
}

void QpTableView::paintEvent(QPaintEvent *event)
{
    Q_D(QpTableView);
    // setup temp variables for the painting

    if( debug ) qDebug()<<"\n---------------------------------------------------------------------------------------------\n                QpTableView::paintEvent \n---------------------------------------------------------------------------------------------\n";

    QStyleOptionViewItemV4 option = d->viewOptionsV4();

    QPainter painter( d->viewport );

    const QPoint offset = d->scrollDelayOffset;

    const bool showGrid = d->showGrid;

    const int gridSize = showGrid ? 1 : 0;

    const int gridHint = style()->styleHint(QStyle::SH_Table_GridLineColor, &option, this);

    const bool  betweenRowsBorder = d->showBetweenRowsBorder;

    //const QColor gridColor = static_cast<QRgb>(gridHint);
    const QColor gridColor = QColor(Qt::green);

    const QPen gridPen = QPen(gridColor, 0, d->gridStyle);

    const QpVertHeaderView *verticalHeader = d->verticalHeader;

    const QpHorHeaderView *horizontalHeader = d->horizontalHeader;

    const bool alternate = d->alternatingColors;

    const bool rightToLeft = isRightToLeft();


    if (horizontalHeader->count() == 0 || verticalHeader->count() == 0 || ! d->itemDelegate)
        return;

    uint x = horizontalHeader->length();
    x -= horizontalHeader->offset();

    int len_y = verticalHeader->length();
    uint y = len_y - verticalHeader->offset() - 1;


    if (  debug_paint_region)  qDebug() << "event->region():" << event->region();


    const QRegion region = event->region().translated(offset);

    const QVector<QRect> rects = region.rects();

    //    if( debug_paint ) qDebug() << "-----------------------------------------------";
    //    if( debug_paint ) qDebug() << "QpTableView::paintEvent ";

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
        lastVisualRow = d->model->rowCount(d->root) - 1;

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

    if ( debug_paint ) qDebug() << "QBitArray drawn " << drawn.size() << " num_y_cnt:"<<num_y_cnt << " num_x_cnt:"<<num_x_cnt << "  lines_count:" <<lines_count;

    //drawn.resize( );

    // -------------------------------------------------------------------------------

    if ( debug_paint ) qDebug() << "QBitArray drawn : " << drawn;

    //    QString str = "\n\t\t";

    //    for( int ii =0; ii < drawn.count(); ii++)
    //    {
    //        int dd = ii % 10 ;

    //        if( dd == 0)
    //            str.append("\n\t\t");

    //        str.append( QString::number(drawn.at( ii )) ).append(" ");

    //    }

    //    if ( debug_paint ) qDebug() << str <<"\n";

    // -------------------------------------------------------------------------------

    for (int i = 0; i < rects.size(); ++i)
    {
        QRect dirtyArea = rects.at(i);

        if( debug_paint_region ) qDebug() << "dirtyArea rect: " << dirtyArea << "  firstVisualRow:" << firstVisualRow;

        dirtyArea.setBottom( qMin(dirtyArea.bottom(), int( y )));

        dirtyArea.setRight( qMin(dirtyArea.right(), int( x )));


        int left_num = horizontalHeader->visual_xNum_At( dirtyArea.left() );

        if( debug_scroll )
            qDebug() << "   left_num:" << left_num << "   dirtyArea.left() " << dirtyArea.left() << " dirtyArea " << dirtyArea;

        int right_num = horizontalHeader->visual_xNum_At( dirtyArea.right()  );


        if (left_num == -1)
            left_num = 0;

        if (right_num == -1)
            right_num = horizontalHeader->xNum_count() - 1;


        // get the vertical start and end visual sections and if alternate color
        int row_bottom = verticalHeader->visualIndexAt( dirtyArea.bottom() );

        if (row_bottom == -1)
        {
            int btm = verticalHeader->count() - 1;
            row_bottom = btm;
        }

        int row_top = 0;

        bool alternateBase = false;

        row_top = verticalHeader->visualIndexAt( dirtyArea.top() );

        if (row_top == -1 || row_top > row_bottom)
            continue;


        if( debug_scroll)  qDebug() << "QpTableView::paintEvent left_num " << left_num << "  right_num " << right_num;
        if( debug_scroll)  qDebug() << "QpTableView::paintEvent row_top " << row_top << "  row_bottom " << row_bottom;

        //----------------------------------------------------
        //              Paint each row item
        //----------------------------------------------------

        //QPen old = painter.pen();
        QPainter painterGrid (d->viewport);

        painterGrid.setPen(gridPen);

        int section_count = horizontalHeader->count_of_section();

        for ( int row = row_top; row <= row_bottom; row++)
        {
            // --------------------------------------------------------------------
            //                      rows
            // --------------------------------------------------------------------

            if( debug_paint_row_col ) qDebug()<< "    row:"<<row<<" row_top:"<<row_top << " row_bottom:"<<row_bottom<< " row - row_top  : " << row - row_top  ;

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

                    qp::SECTION_D dd = horizontalHeader->section_d_at_Num_x_line( numX , line );

                    int sectionNum = dd.number;
                    //int sectionNum = horizontalHeader->logicalIndex_atNum_x_line( numX , line );

                    if( sectionNum == qp::UNDEFINED_FLD)
                        continue;


                    int rowX = horizontalHeader->xNumPosition( left_num );

                    rowX += offset.x();

                    QRect rect;

                    if( sectionNum <= qp::LABEL_FLD)
                    {
                        // ------------------------------------------------------------------------
                        //                              LABEL_FLD
                        // ------------------------------------------------------------------------

                        rect = horizontalHeader->cellRect( line , numX );

                        if( debug_resize ) qDebug() << "LABEL_FLD    rect " << rect << " rowY " << rowY << " rowX " << rowX;

                        if ( debug_scroll )
                            if( offset.y()!=0 || offset.x()!=0 )
                                qDebug() << " offset.y()" << offset.y() << " offset.x() " <<offset.x();

                        int x1 = rect.left();
                        int x2 = rect.right();
                        int y1 = rect.top();
                        int y2 = rect.bottom();

                        rect.setBottom( y2 - gridSize + correct_height_minus_1 );
                        rect.setRight(  x2 - gridSize + correct_width_minus_1 );

                        rect.moveTop( rowY + y1 );
                        //rect.moveLeft( rowX + x1 );

                        option.rect =  rect  ;

                        qp::SECTION_D var = horizontalHeader->get_cell_at_line_xNum( line , numX );

                        if( selectionBehavior()  == QpAbstractItemView::SelectRows)
                        {

                        }
                        bool rowSel = verticalHeader->is_rowSelected( row );


                        if( debug_selection && rowSel )
                        {
                            if ( debug_paint ) qDebug() <<"        rowSelected " << rowSel << " row "<< row;
                        }

                        if( debug_paint_row_col ) qDebug() << "               numX:"  << numX << "  label : "<< var.number;

                        if( debug_paint_row_col )
                            qDebug() << "                   option.rect " << option.rect;


                        qp::CELL_STYLE stl;

                        QStyleOptionViewItemV4 option2 = option;

                        if ( ! dd.txt.isEmpty() )
                            option2.text = dd.txt;

                        if ( horizontalHeader->get_section_style( sectionNum , stl) )
                        {

                            d->correct_Style( stl , option2 );
                            //qDebug()<< "-- stl : "<< line << numX << stl.color.name() << stl.font;

                            d->drawCellLabel( &painter, option2, rowSel , row );
                        }
                        else
                            d->drawCellLabel( &painter, option2,  rowSel , row );


                    }
                    else if (sectionNum >=0 )
                    {
                        // ------------------------------------------------------------------------
                        //                              MODEL_FLD
                        // ------------------------------------------------------------------------

                        if ( horizontalHeader->isSectionHidden( sectionNum ))
                            continue;

                        int currentBit = ( row - firstVisualRow ) * lgcl_max_num  + sectionNum  ;

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


                        rect =  columnViewportPosition2( sectionNum );

                        if ( ! rect.isValid() )
                        {
                            qDebug() << "wrong rect for logicalIndex:"<<sectionNum;
                            continue;
                        }

                        int x1=rect.left();
                        int x2=rect.right();
                        int y1=rect.top();
                        int y2=rect.bottom() ;

                        rect.setBottom( y2 - gridSize + correct_height_minus_1 );
                        rect.setRight(  x2 - gridSize + correct_width_minus_1 );


                        const QModelIndex index = d->model->index( row, sectionNum , d->root);


                        if ( ! index.isValid())
                        {
                            qDebug() << tblName()<< " ! index.isValid() index " << index <<  d->model->data( index).toString() << "  line " << line << " logicalIndex " << sectionNum << " numX " << numX;
                            continue;
                        }

                        if( debug_paint_row_col ) qDebug() << "               numX:"  << numX << "  col : "<<sectionNum;
                        rect.moveTop( rowY + y1 );

                        option.rect = rect ;

                        if( debug_resize ) qDebug() << "        rect " << rect <<  d->model->data( index).toString();;

                        if (alternate)
                        {
                            if (alternateBase)
                                option.features |= QStyleOptionViewItemV2::Alternate;
                            else
                                option.features &= ~QStyleOptionViewItemV2::Alternate;
                        }

                        if( debug_paint_row_col )
                            qDebug() << "                   option.rect " << option.rect <<  d->model->data( index).toString();


                        //-------------------------------------------------------------
                        qp::CELL_STYLE stl;

                        //if ( horizontalHeader->get_cell_style( line , numX , stl) )
                        if ( horizontalHeader->get_section_style( sectionNum , stl) )
                        {
                            QStyleOptionViewItemV4 option2 = option;

                            d->correct_Style( stl , option2 );

                            //qDebug()<< "stl " << line << numX << stl.color.name() << stl.font;

                            d->drawCell( &painter, option2, index  );
                        }
                        else
                            d->drawCell( &painter, option, index  );
                        //-------------------------------------------------------------
                    }



                    if( betweenRowsBorder )
                    {
                        //-------------------------------------------------------------
                        //  between rows border
                        //-------------------------------------------------------------

                        bool draw_bottom = false;

                        if( sectionNum >=0 )
                        {
                            qp::CELL_NODES nd = horizontalHeader->get_logicalIdex_nodes( sectionNum );

                            if( nd.bottom == lines_count  )
                            {
                                draw_bottom = true;
                            }

                        }

                        if ( sectionNum <= qp::LABEL_FLD)
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

int QpTableView::get_section_at( const QPoint & pos)
{
    Q_D(QpTableView);

    int logicalIndex =  horizontalHeader()->logicalIndexAt_xy( pos );

    return logicalIndex;
}

qp::CELL QpTableView::get_cell_at( const QPoint & pos)
{
    Q_D(QpTableView);

    int line =  horizontalHeader()->get_section_line( pos.y() );
    int xNum =  horizontalHeader()->get_section_num( pos.x() );
    //int xNum =  horizontalHeader()->get_section_line();

    qp::SECTION_D dd =horizontalHeader()->get_cell_at_line_xNum( line , xNum);

    qp::CELL cell(line , xNum);

    cell.sectionNum = dd.number;

    return cell;
}


void QpTableView::mousePressEvent(QMouseEvent *event)
{
    Q_D(QpTableView);

    d->delayedAutoScroll.stop(); //any interaction with the view cancel the auto scrolling

    QPoint pos = event->pos();

    QModelIndex idx = indexAt( pos ).idx;

    if( idx.column() == qp::LABEL_FLD )
    {
        if( selectionBehavior() == QpAbstractItemView::SelectRows )
        {
            selectRow( idx.row());
            return;
        }

    }
    QpAbstractItemView::mousePressEvent(event);

}

qp::SECTION QpTableView::indexAt(const QPoint &pos) const
{
    Q_D(const QpTableView);

    // ------------------------------------------------------
    //    this is called from user interactive actions only
    // ------------------------------------------------------

    d->executePostedLayout();

    int row = rowAt( pos.y()); // ok

    //int line = d->horizontalHeader->get_section_line( pos.y() );

    int rowH = rowHeight( row );

    int x = pos.x();
    int y = pos.y();

    int col = columnAt( x , y ) ; // columnAt uses only here (nothing anymore)

    qp::SECTION section;

    if( col == qp::LABEL_FLD)
    {
        QModelIndex idx = d->model->index(row, 0, d->root);

        section.type = qp::LABEL_TYPE;
        section.idx = idx;

        return  section;
    }
    else if ( row >= 0 && col >= 0 )
    {
        QModelIndex idx = d->model->index(row, col, d->root);

        if( debug_selection ) qDebug() << "   QpTableView::indexAt pos "<<pos << " idx " << idx;

        section.type = qp::MODEL_TYPE;
        section.idx = idx;

        return section;
    }

    return section;
}

/*!
            Returns the horizontal offset of the items in the table view.

            Note that the table view uses the horizontal header section
            positions to determine the positions of columns in the view.

            \sa verticalOffset()
        */
int QpTableView::horizontalOffset() const
{
    Q_D(const QpTableView);

    return d->horizontalHeader->offset();
}

/*!
            Returns the vertical offset of the items in the table view.

            Note that the table view uses the vertical header section
            positions to determine the positions of rows in the view.

            \sa horizontalOffset()
        */
int QpTableView::verticalOffset() const
{
    Q_D(const QpTableView);
    return d->verticalHeader->offset();
}

/*!
            \fn QModelIndex QpTableView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)

            Moves the cursor in accordance with the given \a cursorAction, using the
            information provided by the \a modifiers.

            \sa QpAbstractItemView::CursorAction
        */
QModelIndex QpTableView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_D(QpTableView);
    Q_UNUSED(modifiers);

    qDebug() << "QpTableView::moveCursor cursorAction: " <<  cursorAction;

    int bottom = d->model->rowCount(d->root) - 1;
    // make sure that bottom is the bottommost *visible* row
    while (bottom >= 0 && isRowHidden(d->logicalRow(bottom)))
        --bottom;

    int right = d->model->columnCount(d->root) - 1;

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

        return d->model->index(d->logicalRow(row), d->logicalColumn(column), d->root);
    }

    // Update visual cursor if current index has changed.
    QPoint visualCurrent(d->visualColumn(current.column()), d->visualRow(current.row()));
    if (visualCurrent != d->visualCursor)
    {
        d->visualCursor = visualCurrent;
    }

    int visualRow = d->visualCursor.y();

    if (visualRow > bottom)
        visualRow = bottom;

    Q_ASSERT(visualRow != -1);

    int visualColumn = d->visualCursor.x();

    if (visualColumn > right)
        visualColumn = right;

    Q_ASSERT(visualColumn != -1);

    switch (cursorAction)
    {
    case MoveUp:
    {
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
            //            if (firstTime && c != -1 && d->hasSpans())
            //            {
            //                firstTime = false;
            ////                QSpanCollection2::Span span = d->span(r, c);
            ////                if (span.width() > 1 || span.height() > 1)
            ////                    visualColumn = d->visualColumn(span.left());
            //            }

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
        int newRow = rowAt(visualRect(current).top() - d->viewport->height());
        if (newRow == -1)
            newRow = d->logicalRow(0);
        return d->model->index(newRow, current.column(), d->root);
    }
    case MovePageDown: {
        int newRow = rowAt(visualRect(current).bottom() + d->viewport->height());
        if (newRow == -1)
            newRow = d->logicalRow(bottom);
        return d->model->index(newRow, current.column(), d->root);
    }}

    d->visualCursor = QPoint(visualColumn, visualRow);

    int logicalRow = d->logicalRow(visualRow);
    int logicalColumn = d->logicalColumn(visualColumn);
    if (!d->model->hasIndex(logicalRow, logicalColumn, d->root))
        return QModelIndex();

    QModelIndex result = d->model->index(logicalRow, logicalColumn, d->root);
    if (!d->isRowHidden(logicalRow) && !d->isColumnHidden(logicalColumn) && d->isIndexEnabled(result))
        return result;

    return QModelIndex();
}

/*!
            \fn void QpTableView::setSelection(const QRect &rect,
            QItemSelectionModel::SelectionFlags flags)

            Selects the items within the given \a rect and in accordance with
            the specified selection \a flags.
        */
void QpTableView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_D(QpTableView);

    if( debug_selection ) qDebug() << "QpTableView::setSelection rect" << rect;

    QModelIndex tl = indexAt( QPoint( qMin(rect.left(), rect.right()), qMin(rect.top(), rect.bottom()))).idx;


    QModelIndex br = indexAt( QPoint( qMax(rect.left(), rect.right()), qMax(rect.top(), rect.bottom()))).idx;

    //if( debug_selection ) qDebug() << "QpTableView::setSelection rect" << rect << "  tl "<< tl << " br " << br;


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
                QModelIndex index = d->model->index(row, column, d->root);
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
            QModelIndex topLeft = d->model->index(tl.row(), column, d->root);
            QModelIndex bottomRight = d->model->index(br.row(), column, d->root);
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
            QModelIndex topLeft = d->model->index(row, tl.column(), d->root);
            QModelIndex bottomRight = d->model->index(row, br.column(), d->root);
            selection.append(QItemSelectionRange(topLeft, bottomRight));
        }
    }
    else
    { // nothing moved

        d->setHoverIndex( QModelIndex()); // !!?? lifehack

        QItemSelectionRange range(tl, br);

        if (!range.isEmpty())

            selection.append(range);
    }

    if( debug_selection ) qDebug()<< " QpTableView::setSelection selection " << selection;

    d->selectionModel->select(selection, command);

}


QRegion QpTableView::visualRegionForSelection(const QItemSelection &selection) const
{
    if ( debug_selection ) qDebug() << "QpTableView::visualRegionForSelection selection: " << selection ;

    if ( debug_selection )
    {
        foreach (QItemSelectionRange itm, selection)
            if( debug_selection ) qDebug() << "   indices : - " << itm.left() << itm.right() << "   |"<< itm.top() <<itm.bottom();
    }


    Q_D(const QpTableView);

    if (selection.isEmpty())
        return QRegion();

    QRegion selectionRegion;

    const QRect &viewportRect = d->viewport->rect();
    bool verticalMoved = verticalHeader()->sectionsMoved();
    bool horizontalMoved = horizontalHeader()->sectionsMoved();

    if ((verticalMoved && horizontalMoved) )
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            QItemSelectionRange range = selection.at(i);
            if (range.parent() != d->root || !range.isValid())
                continue;
            for (int r = range.top(); r <= range.bottom(); ++r)
                for (int c = range.left(); c <= range.right(); ++c)
                {
                    const QRect &rangeRect = visualRect(d->model->index(r, c, d->root));
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
            if (range.parent() != d->root || !range.isValid())
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

            if (range.parent() != d->root || !range.isValid())
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

            if (range.parent() != d->root || !range.isValid())
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

    if( debug_selection )  qDebug() << "QpTableView::visualRegionForSelection return selectionRegion " << selectionRegion;

    return selectionRegion;
}


QModelIndexList QpTableView::selectedIndexes() const
{
    if( debug_selection ) qDebug() << "QpTableView::selectedIndexes()";

    Q_D(const QpTableView);
    QModelIndexList viewSelected;
    QModelIndexList modelSelected;

    if (d->selectionModel)
        modelSelected = d->selectionModel->selectedIndexes();

    for (int i = 0; i < modelSelected.count(); ++i)
    {
        QModelIndex index = modelSelected.at(i);

        if (!isIndexHidden(index) && index.parent() == d->root)
            viewSelected.append(index);
    }

    return viewSelected;
}


/*!
                    This slot is called whenever rows are added or deleted. The
                    previous number of rows is specified by \a oldCount, and the new
                    number of rows is specified by \a newCount.
                */
void QpTableView::rowCountChanged(int oldCount, int newCount )
{
    Q_D(QpTableView);
    //when removing rows, we need to disable updates for the header until the geometries have been
    //updated and the offset has been adjusted, or we risk calling paintSection for all the sections

    if ( debug_paint ) qDebug() << "QpTableView::rowCountChanged oldCount " <<oldCount << " newCount " << newCount;

    if (newCount < oldCount)
    {
        d->verticalHeader->setUpdatesEnabled(false);

    }
    d->doDelayedItemsLayout(); // !! else not updates vertical header se:ctions
}


void QpTableView::columnCountChanged(int, int)
{
    Q_D(QpTableView);

    updateGeometries();

    if (horizontalScrollMode() == QpAbstractItemView::ScrollPerItem)
        d->horizontalHeader->setOffsetToSectionPosition(horizontalScrollBar()->value());
    else
        d->horizontalHeader->setOffset(horizontalScrollBar()->value());

    d->viewport->update();
}

/*!
                    \reimp
                */

void QpTableView::updateGeometries_()
{
    updateGeometries();
}

void QpTableView::updateGeometries()
{
    Q_D(QpTableView);

    if (d->geometryRecursionBlock)
        return;

    d->geometryRecursionBlock = true;

    int width = 0;

    if ( !d->verticalHeader->isHidden())
    {
        width = qMax(d->verticalHeader->minimumWidth(), d->verticalHeader->sizeHint().width());

        width = qMin(width, d->verticalHeader->maximumWidth());
    }

    int height = d->horizontalHeader->row_height();

    if (! d->horizontalHeader->isHidden())
    {
        height = qMax( d->horizontalHeader->minimumHeight(), height );

        height = qMin( height, d->horizontalHeader->maximumHeight() );

        if ( debug_geometry ) qDebug() << " setViewportMargins width:" << width << " height:" << height << " maximumHeight(): " << d->horizontalHeader->maximumHeight() << " minimumHeight():" <<d->horizontalHeader->minimumHeight();
    }

    setViewportMargins( width, height, 0, 0);


    // update headers

    QRect vg = d->viewport->geometry();

    int verticalLeft = vg.left() - width;

    d->verticalHeader->setGeometry( verticalLeft, vg.top(), width, vg.height());

    if (d->verticalHeader->isHidden())
        QMetaObject::invokeMethod(d->verticalHeader, "updateGeometries");

    int horizontalTop = vg.top() - height;

    d->horizontalHeader->setGeometry( vg.left(), horizontalTop, vg.width(), height);

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
    QSize vsize = d->viewport->size();
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

    int width_ = 0;

    for ( int xNum = first_xNum; xNum <= last_xNum; xNum++)
    {
        int ww = d->horizontalHeader->xNum_size( xNum );

        width_ += ww;


        if (width_ > viewportWidth)

            break;

        ++xNumsInViewport;

    }

    xNumsInViewport = qMax(xNumsInViewport, 1); //there must be always at least 1 column

    if (horizontalScrollMode() == QpAbstractItemView::ScrollPerItem)
    {
        const int visibleColumns = xNumCount - d->horizontalHeader->hiddenSectionCount();


        int rangMax = visibleColumns - xNumsInViewport;

        if( debug_scroll ) qDebug() << " QpTableView::updateGeometries() ";
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

    if (verticalScrollMode() == QpAbstractItemView::ScrollPerItem)
    {
        const int visibleRows = rowCount - d->verticalHeader->hiddenSectionCount();

        verticalScrollBar()->setRange(0, visibleRows - rowsInViewport);

        verticalScrollBar()->setPageStep(rowsInViewport);

        if (rowsInViewport >= visibleRows)
            d->verticalHeader->setOffset(0);

        verticalScrollBar()->setSingleStep(1);

        if( rowCount == 30 )
            qDebug() << "updateGeometries 30";

        if( debug_geometry )qDebug() << "updateGeometries rowCount "  << rowCount << "  rowsInViewport " << rowsInViewport << " visibleRows " << visibleRows;
    }
    else
    { // ScrollPerPixel
        verticalScrollBar()->setPageStep( vsize.height() );
        verticalScrollBar()->setRange(0, verticalLength - vsize.height());
        verticalScrollBar()->setSingleStep(qMax(vsize.height() / (rowsInViewport + 1), 2));
    }

    d->geometryRecursionBlock = false;
    QpAbstractItemView::updateGeometries();
}

int QpTableView::sizeHintForRow(int row) const
{
    Q_D(const QpTableView);

    int rowH = d->horizontalHeader->row_height();

    if( rowH == -1)
        return QpTableView::defaultRowHeight;

    return d->showGrid ? rowH + 1 : rowH;

}

int QpTableView::sizeHintForColumn(int column) const
{
    Q_D(const QpTableView);

    if (!model())
        return -1;

    //int hint = 100;

    ensurePolished();

    int top = qMax(0, d->verticalHeader->visualIndexAt(0));

    int bottom = d->verticalHeader->visualIndexAt(d->viewport->height());

    if (!isVisible() || bottom == -1) // the table don't have enough rows to fill the viewport
        bottom = d->model->rowCount(d->root) - 1;

    QStyleOptionViewItemV4 option = d->viewOptionsV4();

    int hint = 0;
    QModelIndex index;
    for (int row = top; row <= bottom; ++row)
    {
        int logicalRow = d->verticalHeader->logicalIndex(row);
        if (d->verticalHeader->isSectionHidden(logicalRow))
            continue;
        index = d->model->index(logicalRow, column, d->root);

        QWidget *editor = d->editorForIndex(index).widget.data();
        if (editor && d->persistent.contains(editor)) {
            hint = qMax(hint, editor->sizeHint().width());
            int min = editor->minimumSize().width();
            int max = editor->maximumSize().width();
            hint = qBound(min, hint, max);
        }

        hint = qMax(hint, itemDelegate(index)->sizeHint(option, index).width());
    }

    hint = d->showGrid ? hint + 1 : hint;
    if( debug ) qDebug()<< " QpTableView::sizeHintForColumn(column:"<<column<<")" <<  hint;
    return hint;
}

/*!
                    Returns the y-coordinate in contents coordinates of the given \a
                    row.
                */
int QpTableView::rowViewportPosition(int row) const
{
    Q_D(const QpTableView);

    int y = d->verticalHeader->sectionViewportPosition( row ) ;

    //if(debug) qDebug() << "QpTableView::rowViewportPosition y:" << y;

    return y ;
}

/*!
                    Returns the row in which the given y-coordinate, \a y, in contents
                    coordinates is located.

                    \note This function returns -1 if the given coordinate is not valid
                    (has no row).

                    \sa columnAt()
                */
int QpTableView::rowAt(int y) const
{
    Q_D(const QpTableView);

    int row = d->verticalHeader->logicalIndexAt( y ); // QpTableView::lines;

    //if(debug) qDebug() << "QpTableView::rowAt(y:"<<y<<") = " << row;

    return row ;
}


void QpTableView::setLineHeightInRow(int line, int height) const
{
    Q_D(const QpTableView);

    horizontalHeader()->setLineHeightInRow( line , height);


}

void QpTableView::setRowHeight(int row, int height)
{
    Q_D(const QpTableView);
    d->verticalHeader->resizeSection(row, height);
}


int QpTableView::rowHeight(int row) const
{
    Q_D(const QpTableView);

    return d->verticalHeader->sectionSize(row);
}

//int QpTableView::rowLine(int y) const
//{
//    Q_D(const QpTableView);

//    return d->horizontalHeader->sectionSize();
//}

/*!
                    Returns the x-coordinate in contents coordinates of the given \a
                    column.
                */
QRect QpTableView::columnViewportPosition2(int logicalIndex ) const
{
    Q_D(const QpTableView);

    QRect retSection = d->horizontalHeader->sectionViewportPosition2( logicalIndex  );


    return retSection;
}

int QpTableView::columnViewportPosition(int column) const
{
    Q_D(const QpTableView);

    int retSection = d->horizontalHeader->sectionViewportPosition2( column ).x();

    return retSection;
}


int QpTableView::columnAt( int x , int y) const
{
    // -------------------------------------------------------
    // This occures only for user actions, not for paintEvent
    // -------------------------------------------------------

    Q_D(const QpTableView);

    // x - coordinate in viewport

    // y - coordinate in viewport

    // There is sometimes an incomplete first row
    int viewport_top = d->verticalHeader->offset();

    //int gggg= viewport()->yoffset;

    int rowH = d->horizontalHeader->row_height();

    if( rowH == qp::UNKNOWN_VALUE )
        return rowH;

    //    int incomplete_first_visible_row = viewport_top %  rowH;

    //    if ( incomplete_first_visible_row > 0 )
    //    {

    //        int revert =  rowH - incomplete_first_visible_row;

    //        y -= revert;

    //        if( debug ) qDebug() << " incomplete_first_visible_row " << incomplete_first_visible_row << "  revert " << revert << "  y " << y;
    //    }



    return d->horizontalHeader->logicalIndexAt_xy(QPoint( x , y ) ); //?????
}

//PblSqlRelationalTableModel* QpTableView::Model() const
//{
//    Q_D(const QpTableView);

//    if ( d->model == QAbstractItemModelPrivate::staticEmptyModel() )
//        return 0;

//    PblSqlRelationalTableModel* rtm = qobject_cast<PblSqlRelationalTableModel* >( d->model );

//    if( ! rtm )
//        return 0;

//    return rtm;

//}

const QString QpTableView::tblName() const
{
    Q_D(const QpTableView);

    if( ! d->model || d->model == QAbstractItemModelPrivate::staticEmptyModel())
        return mdlIsEmptyStr;

    QSqlTableModel* tm = qobject_cast<QSqlTableModel* >( d->model );

    if( ! tm )
        return "";

    return tm->tableName();

}

void QpTableView::setColumnWidth(int column, int width)
{
    Q_D(const QpTableView);
    //d->horizontalHeader->resizeSection(column, width);
}

int QpTableView::columnWidth(int column) const
{
    Q_D(const QpTableView);

    return d->horizontalHeader->sectionSize(column);
}

bool QpTableView::isRowHidden(int row) const
{
    Q_D(const QpTableView);
    return d->verticalHeader->isSectionHidden(row);
}

void QpTableView::setRowHidden(int row, bool hide)
{
    Q_D(QpTableView);
    if (row < 0 || row >= d->verticalHeader->count())
        return;
    d->verticalHeader->setSectionHidden(row, hide);
}

bool QpTableView::isColumnHidden(int column) const
{
    Q_D(const QpTableView);
    return d->horizontalHeader->isSectionHidden(column);
}

void QpTableView::setColumnHidden(int column, bool hide)
{
    Q_D(QpTableView);
    if (column < 0 || column >= d->horizontalHeader->count())
        return;
    d->horizontalHeader->setSectionHidden(column, hide);
}

/*!
                    \since 4.2
                    \property QpTableView::sortingEnabled
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
void QpTableView::setSortingEnabled(bool enable)
{
    Q_D(QpTableView);
    d->sortingEnabled = enable;
    horizontalHeader()->setSortIndicatorShown(enable);
    if (enable)
    {
        disconnect(d->horizontalHeader, SIGNAL(sectionEntered(int)),
                   this, SLOT(_q_selectColumn(int)));

        disconnect(horizontalHeader(), SIGNAL(sectionPressed(int)),
                   this, SLOT(selectColumn(int)));

        connect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
                this, SLOT(sortByColumn(int)), Qt::UniqueConnection);

        sortByColumn(horizontalHeader()->sortIndicatorSection(),
                     horizontalHeader()->sortIndicatorOrder());
    }
    else
    {
        connect(d->horizontalHeader, SIGNAL(sectionEntered(int)),
                this, SLOT(_q_selectColumn(int)), Qt::UniqueConnection);

        connect(horizontalHeader(), SIGNAL(sectionPressed(int)),
                this, SLOT(selectColumn(int)), Qt::UniqueConnection);

        disconnect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
                   this, SLOT(sortByColumn(int)));
    }
}

bool QpTableView::isSortingEnabled() const
{
    Q_D(const QpTableView);
    return d->sortingEnabled;
}


bool QpTableView::showGrid() const
{
    Q_D(const QpTableView);

    return d->showGrid;
}

bool QpTableView::showBetweenRowBorder() const
{
    Q_D(const QpTableView);

    return d->showBetweenRowsBorder;

}

void QpTableView::setShowBetweenRowBorder(bool show)
{
    Q_D(QpTableView);

    if ( d->showBetweenRowsBorder != show)
    {
        d->showBetweenRowsBorder = show;
        d->viewport->update();
    }

    //horizontalHeader()->setGridWidth( 1 );
}


void QpTableView::setShowGrid(bool show)
{
    Q_D(QpTableView);

    if ( d->showGrid != show)
    {
        d->showGrid = show;
        d->viewport->update();
    }

    horizontalHeader()->setGridWidth( 1 );
}

/*!
                  \property QpTableView::gridStyle
                  \brief  the pen style used to draw the grid.

                  This property holds the style used when drawing the grid (see \l{showGrid}).
                */
Qt::PenStyle QpTableView::gridStyle() const
{
    Q_D(const QpTableView);

    return d->gridStyle;
}

void QpTableView::setRowGridStyle(Qt::PenStyle style)
{

    Q_D(QpTableView);

    if (d->gridStyle != style)
    {
        d->gridStyle = style;
        d->viewport->update();
    }
}

void QpTableView::setGridStyle(Qt::PenStyle style)
{
    Q_D(QpTableView);

    if (d->gridStyle != style)
    {
        d->gridStyle = style;
        d->viewport->update();
    }
}

void QpTableView::setWordWrap(bool on)
{
    Q_D(QpTableView);
    if (d->wrapItemText == on)
        return;
    d->wrapItemText = on;
    QMetaObject::invokeMethod(d->verticalHeader, "resizeSections");
    QMetaObject::invokeMethod(d->horizontalHeader, "resizeSections");
}

bool QpTableView::wordWrap() const
{
    Q_D(const QpTableView);
    return d->wrapItemText;
}


void QpTableView::setCornerButtonEnabled(bool enable)
{
    Q_D(QpTableView);
    d->cornerWidget->setEnabled(enable);
}

bool QpTableView::isCornerButtonEnabled() const
{
    Q_D(const QpTableView);
    return d->cornerWidget->isEnabled();
}

/*!
                    \internal

                    Returns the rectangle on the viewport occupied by the given \a
                    index.
                    If the index is hidden in the view it will return a null QRect.
                */
QRect QpTableView::visualRect(const QModelIndex &index) const
{
    // ------------------------------------------------------
    // this is called from user doubleclick on a cell (for example)
    // ------------------------------------------------------
    Q_D(const QpTableView);

    if ( ! d->isIndexValid(index)
         || index.parent() != d->root
         || isIndexHidden(index) )

        return QRect();

    d->executePostedLayout();

    int row_viewpport_y = rowViewportPosition(index.row());

    QRect rect = columnViewportPosition2(index.column());

    int y = rect.y();

    int offset_y = row_viewpport_y + y;

    //if( debug ) qDebug() << "QpTableView::visualRect rect " << rect << " row_viewpport_y " << row_viewpport_y << "  offset_y " << offset_y;

    rect.moveTop( offset_y );

    //int colw = columnWidth(index.column());

    const int i = showGrid() ? 1 : 0;

    //if( debug ) qDebug() << "QpTableView::visualRect return rect " << rect;

    return rect;
}

void QpTableView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_D(QpTableView);

    if( debug_scroll ) qDebug() << "QpTableView::scrollTo " << index << " ScrollHint hint " << hint;

    // check if we really need to do anything
    if (!d->isIndexValid(index)
            || (d->model->parent(index) != d->root)
            || isRowHidden(index.row()) || isColumnHidden(index.column()))
        return;

    // Adjust horizontal position

    int viewportWidth = d->viewport->width();
    int horizontalOffset = d->horizontalHeader->offset();

    int horizontalPosition = d->horizontalHeader->sectionPosition2(index.column()).x(); //!!

    int horizontalIndex = d->horizontalHeader->visualIndex(index.column());

    //    int cellWidth = d->hasSpans()
    //            ? d->columnSpanWidth(index.column(), span.width())
    //            : d->horizontalHeader->sectionSize(index.column());
    int cellWidth =  d->horizontalHeader->sectionSize(index.column());

    if (horizontalScrollMode() == QpAbstractItemView::ScrollPerItem)
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

    int viewportHeight = d->viewport->height();
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

    if (verticalScrollMode() == QpAbstractItemView::ScrollPerItem)
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

void QpTableView::rowResized(int row, int oldHeight, int newHeight)
{
    Q_D(QpTableView);
    d->rowsToUpdate.append(row);

    if (d->rowResizeTimerID == 0)
        d->rowResizeTimerID = startTimer(0);
}

void QpTableView::linesInRowResized_Y()
{
    Q_D(QpTableView);

    int hh = horizontalHeader()->row_height();

    verticalHeader()->setDefaultSectionSize( hh );

    int firstVisualRow = qMax(verticalHeader()->visualIndexAt( 0 ) , 0);

    int lastVisualRow = verticalHeader()->visualIndexAt(verticalHeader()->viewport()->height() );

    for( int row = firstVisualRow; row<=lastVisualRow; row++ )
        d->rowsToUpdate.append(row);

    if (d->rowResizeTimerID == 0)
        d->rowResizeTimerID = startTimer(0);

}

void QpTableView::xNumsResized_X(int xNum)
{
    Q_D(QpTableView);

    d->xNumsToUpdate.append( xNum );

    if (d->xNumsResizeTimerID == 0)
        d->xNumsResizeTimerID = startTimer(0);
}


void QpTableView::timerEvent(QTimerEvent *event)
{
    Q_D(QpTableView);

    // -----------------------------------------------------------------------
    // This timer is only for repainting with mouse changes section sizes
    // -----------------------------------------------------------------------

    if (event->timerId() == d->xNumsResizeTimerID)
    {
        if( debug_event ) qDebug() << "QpTableView::timerEvent xNumsResizeTimerID";

        updateGeometries();

        killTimer(d->xNumsResizeTimerID);

        d->xNumsResizeTimerID = 0;

        QRect rect;
        int viewportHeight = d->viewport->height();
        int viewportWidth = d->viewport->width();


        for (int i = d->xNumsToUpdate.size()-1; i >= 0; --i)
        {
            int xNum = d->xNumsToUpdate.at(i);

            int x = horizontalHeader()->xNum_left( xNum );//  left_common_border_x( xNum ) ;

            if( x == qp::UNKNOWN_VALUE )
                continue;

            rect |= QRect(x, 0, viewportWidth - x, viewportHeight);

            qDebug() << "xNumsResizeTimerID rect "<<rect;
        }

        d->horizontalHeader->viewport()->updateGeometry();
        d->horizontalHeader->viewport()->update();

        //updateGeometries();
        //update();
        d->viewport->update( );//rect.normalized() );
        d->xNumsToUpdate.clear();
    }

    if (event->timerId() == d->rowResizeTimerID)
    {
        if( debug_event ) qDebug() << "QpTableView::timerEvent rowResizeTimerID";

        updateGeometries();

        killTimer(d->rowResizeTimerID);

        d->rowResizeTimerID = 0;

        int viewportHeight = d->viewport->height();
        int viewportWidth = d->viewport->width();
        int top;

        top = viewportHeight;

        for (int i = d->rowsToUpdate.size()-1; i >= 0; --i)
        {
            int y = rowViewportPosition(d->rowsToUpdate.at(i));
            top = qMin(top, y);
        }

        d->viewport->update(QRect(0, top, viewportWidth, viewportHeight - top));

        d->rowsToUpdate.clear();
    }

    if (event->timerId() == delayed_Repaint_tmr)
    {
        if( debug_selection | debug_event ) qDebug() << "QpTableView::timerEvent delayed_Repaint_tmr";

        killTimer( delayed_Repaint_tmr);

        delayed_Repaint_tmr = 0;

        d->horizontalHeader->viewport()->updateGeometry();
        d->horizontalHeader->viewport()->update();

        d->verticalHeader->viewport()->updateGeometry();
        d->verticalHeader->viewport()->update();

        qDebug() << "offset:"<< verticalHeader()->offset() ;

        //verticalHeader()->setOffset( 100 );

        //qDebug() << "offset:"<< verticalHeader()->offset() ;

        updateGeometries();
        update();

    }

    QpAbstractItemView::timerEvent(event);
}

/*!
                    This slot is called to change the index of the given \a row in the
                    table view. The old index is specified by \a oldIndex, and the new
                    index by \a newIndex.

                    \sa columnMoved()
                */
void QpTableView::rowMoved(int, int oldIndex, int newIndex)
{
    Q_D(QpTableView);

    updateGeometries();
    int logicalOldIndex = d->verticalHeader->logicalIndex(oldIndex);
    int logicalNewIndex = d->verticalHeader->logicalIndex(newIndex);

    //    if (d->hasSpans())
    //    {
    //        d->viewport->update();
    //    }
    //    else
    //    {
    int oldTop = rowViewportPosition(logicalOldIndex);
    int newTop = rowViewportPosition(logicalNewIndex);
    int oldBottom = oldTop + rowHeight(logicalOldIndex);
    int newBottom = newTop + rowHeight(logicalNewIndex);
    int top = qMin(oldTop, newTop);
    int bottom = qMax(oldBottom, newBottom);
    int height = bottom - top;
    d->viewport->update(0, top, d->viewport->width(), height);
    //    }
}

/*!
                    This slot is called to change the index of the given \a column in
                    the table view. The old index is specified by \a oldIndex, and
                    the new index by \a newIndex.

                    \sa rowMoved()
                */
void QpTableView::columnMoved(int, int oldIndex, int newIndex)
{
    Q_D(QpTableView);

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

    d->viewport->update(left, 0, width, d->viewport->height());
}

void QpTableView::selectRow(int row)
{
    Q_D(QpTableView);

    //horizontalHeader()->clearSelection(); //!!

    d->selectRow(row, true);
}

void QpTableView::selectColumn(int column)
{
    Q_D(QpTableView);
    d->selectColumn(column, true);
}

void QpTableView::hideRow(int row)
{
    Q_D(QpTableView);
    d->verticalHeader->hideSection(row);
}

void QpTableView::hideColumn(int column)
{
    Q_D(QpTableView);
    d->horizontalHeader->hideSection(column);
}

void QpTableView::showRow(int row)
{
    Q_D(QpTableView);
    d->verticalHeader->showSection(row);
}

void QpTableView::showColumn(int column)
{
    Q_D(QpTableView);
    d->horizontalHeader->showSection(column);
}

void QpTableView::reset()
{
    QpAbstractItemView::reset();
    //verticalHeader()->reset();
}

void QpTableView::resizeRowToContents(int row)
{
    Q_D(QpTableView);

    int content = sizeHintForRow(row);

    int header = d->verticalHeader->sectionSizeHint(row);

    d->verticalHeader->resizeSection(row, qMax(content, header));
}

/*!
                    Resizes all rows based on the size hints of the delegate
                    used to render each item in the rows.
                */
void QpTableView::resizeRowsToContents()
{
    Q_D(QpTableView);

    d->verticalHeader->resizeSections( QpVertHeaderView::ResizeToContents );
}

void QpTableView::resizeColumnToContents(int column)
{
    //    Q_D(QpTableView);

    //    int content = sizeHintForColumn(column);

    //    int header = d->horizontalHeader->sectionSizeHint(column);

    //    d->horizontalHeader->resizeSection(column, qMax(content, header));
}

void QpTableView::resizeColumnsToContents()
{
    Q_D(QpTableView);
    d->horizontalHeader->resizeSections(QpHorHeaderView::ResizeToContents);
}

void QpTableView::sortByColumn(int column)
{
    Q_D(QpTableView);
    if (column == -1)
        return;
    d->model->sort(column, d->horizontalHeader->sortIndicatorOrder());
}

void QpTableView::sortByColumn(int column, Qt::SortOrder order)
{
    Q_D(QpTableView);
    d->horizontalHeader->setSortIndicator(column, order);
    sortByColumn(column);
}

void QpTableView::verticalScrollbarAction(int action)
{
    if( debug_scroll) qDebug() << "QpTableView::verticalScrollbarAction";

    QpAbstractItemView::verticalScrollbarAction(action); // NOTHING
}

void QpTableView::horizontalScrollbarAction(int action)
{
    if( debug_scroll) qDebug() << "QpTableView::horizontalScrollbarAction " << action;

    QpAbstractItemView::horizontalScrollbarAction(action);
}

bool QpTableView::isIndexHidden(const QModelIndex &index) const
{
    Q_D(const QpTableView);
    Q_ASSERT(d->isIndexValid(index));

    if (isRowHidden(index.row()) || isColumnHidden(index.column()))
        return true;

    return false;
}


void QpTableViewPrivate::_q_selectRow(int row)
{
    selectRow(row, false);
}

void QpTableViewPrivate::_q_selectColumn(int column)
{
    selectColumn(column, false);
}

void QpTableViewPrivate::selectRow(int row, bool anchor)
{
    if( debug_selection ) qDebug() << "QpTableViewPrivate::selectRow row:" << row;

    Q_Q(QpTableView);

    if (q->selectionBehavior() == QpTableView::SelectColumns
            || (q->selectionMode() == QpTableView::SingleSelection
                && q->selectionBehavior() == QpTableView::SelectItems))
        return;

    if( model->columnCount() <=0)

        return;

    if (row >= 0 && row < model->rowCount(root))
    {
        // horizontalHeader->logicalIndexAt( QPoint( 0 , 0 ) ); //??????

        int column = 0; // !!!!! column 0 has to be forever

        QModelIndex index = model->index(row, column, root);

        QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);

        qDebug() << "QpTableViewPrivate::selectRow command:" << command;

        selectionModel->setCurrentIndex(index, QItemSelectionModel::NoUpdate);

        if ((anchor && !(command & QItemSelectionModel::Current))
                || (q->selectionMode() == QpTableView::SingleSelection))

            rowSectionAnchor = row;

        if (q->selectionMode() != QpTableView::SingleSelection
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


bool QpTableView::is_RowSelected(int row)
{
    Q_D(QpTableView);

    qDebug() << "QpTableViewPrivate::is_RowSelected row:" << row << " rowSectionAnchor:" << d->rowSectionAnchor ;

    if ( row == d->rowSectionAnchor)
        return true;

    return false;
}

void QpTableViewPrivate::selectColumn(int column, bool anchor)

{
    if( debug_selection ) qDebug() << "QpTableViewPrivate::selectColumn column:" << column;

    Q_Q(QpTableView);

    if (q->selectionBehavior() == QpTableView::SelectRows
            || (q->selectionMode() == QpTableView::SingleSelection
                && q->selectionBehavior() == QpTableView::SelectItems))

        return;

    if (column >= 0 && column < model->columnCount(root))
    {
        int row = verticalHeader->logicalIndexAt( 0 );

        QModelIndex index = model->index(row, column, root);

        QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);

        selectionModel->setCurrentIndex(index, QItemSelectionModel::NoUpdate);

        if ((anchor && !(command & QItemSelectionModel::Current))
                || (q->selectionMode() == QpTableView::SingleSelection))
            columnSectionAnchor = column;

        if (q->selectionMode() != QpTableView::SingleSelection
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


void QpTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    //qDebug() << "QpTableView::currentChanged not debugging current "<< current << "  previous "<<previous;

#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive())
    {
        if (current.isValid())
        {
#ifdef Q_WS_X11
            Q_D(QpTableView);
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
    QpAbstractItemView::currentChanged(current, previous);
}


void QpTableView::selectionChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
{
    if ( debug_selection )
    {
        qDebug() << "QpTableView::selectionChanged selected:" << selected;
        qDebug() <<  " deselected:"<<deselected;
    }
    Q_D(QpTableView);

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

    QpAbstractItemView::selectionChanged(selected, deselected);
}

void QpTableView::updateGeometry()
{
    qDebug() << "QpTableView::updateGeometry";

    QWidget::updateGeometry();
}

int QpTableView::visualIndex(const QModelIndex &index) const
{
    return index.row();
}

QFont QpTableView::get_section_font( int mdlfldNum ) const
{
    Q_D( const QpTableView);

    //QPainter painter( d->viewport );

    QStyleOptionViewItemV4 opt = d->viewOptionsV4();

    return opt.font;
}

void QpTableViewPrivate::correct_Style( const qp::CELL_STYLE &stls,
                                        QStyleOptionViewItemV4 &opt)
{
    //QPainter painter( viewport );

    //QStyleOptionViewItemV4 option = d->viewOptionsV4();
    //opt = viewOptionsV4();

    if( opt.font != stls.font)
    {
        opt.font = stls.font;

        //qDebug() << "correctStyle fnt" << opt.font;

    }

    if( stls.color.isValid() && stls.color != opt.palette.color( QPalette::Text ) )
    {
        opt.palette.setColor( QPalette::Text,  stls.color );
    }

    if( stls.align != opt.displayAlignment )
        opt.displayAlignment =  stls.align;

    //    if( ! stls.txt.isEmpty() )
    //        opt.text = stls.txt;

}

const qp::CELL_STYLE QpTableView::get_section_default_style( int line, int numX ) const
{
    Q_D( const QpTableView);

    const QStyleOptionViewItemV4 opt = d->viewOptionsV4();

    qp::CELL_STYLE stl;
    stl.font = opt.font;
    stl.color = opt.palette.color(QPalette::Text);
    stl.align = opt.displayAlignment;
    return stl;

}


Qt::Alignment QpTableView::get_section_align( int line, int numX ) const
{
    Q_D( const QpTableView);

    //    const qp::CELL cell( line, numX);

    //    if( d->cell_styles.contains( cell ))

    //        return d->cell_styles.value( cell ).align ;

    return Qt::AlignCenter;
}

bool QpTableView::set_section_align( int mdlfldNum, Qt::Alignment align )
{
    Q_D(QpTableView);

    d->stls[ mdlfldNum ].align = align;

    qDebug() << "QpTableView::set_align_section : " << d->stls[ mdlfldNum ].align;

    return true;
}

bool QpTableView::init_template( const qp::SECTIONS_TMPL &matrix )
{
    //reset();


    horizontalScrollBar()->setValue( 0 ); //!!
    verticalScrollBar()->setValue( 0 ); //!!

    clearSelection();

    if( horizontalHeader()->init_sections_template( matrix ) )
    {

        //horizontalHeader()->updateGeometry();
        //verticalHeader()->updateGeometry(); !!! not here
        //updateGeometry(); //!!

        delayed_Repaint();//!!
        //doItemsLayout();

        return true;
    }

    return false;
}

//void QpTableView::slot_rowsInserted( const QModelIndex &parent, int first, int last)
//{
//    if( debug ) qDebug() << "QpTableView::slot_rowsInserted first " << first << " last " <<last;
//    //qDebug() << "    rowHeight   " << rowHeight( );

//}



void QpTableViewPrivate::_q_updateSpanInsertedRows(const QModelIndex &parent, int start, int end)
{
    if( debug_mdl_signals ) qDebug() << "QpTableViewPrivate::_q_updateSpanInsertedRows( start " << start << ", end " << end<<") nothing, verticalHeader->length()"<< verticalHeader->length();

    Q_UNUSED(parent)
    //spans.updateInsertedRows(start, end);
}

void QpTableViewPrivate::_q_updateSpanInsertedColumns(const QModelIndex &parent, int start, int end)
{
    if( debug_mdl_signals ) qDebug() << "QpTableViewPrivate::_q_updateSpanInsertedColumns( start " << start << ", end " << end<<") nothing, verticalHeader->length()"<<verticalHeader->length();

    Q_UNUSED(parent)
    //spans.updateInsertedColumns(start, end);
}

void QpTableViewPrivate::_q_updateSpanRemovedRows(const QModelIndex &parent, int start, int end)
{
    if( debug_mdl_signals ) qDebug() << "QpTableViewPrivate::_q_updateSpanRemovedRows( first " << start << ", last " << end<<") nothing sectionCount " << verticalHeader->count() << " verticalHeader->length()"<<verticalHeader->length();
    Q_UNUSED(parent)
    //spans.updateRemovedRows(start, end);
}

void QpTableViewPrivate::_q_updateSpanRemovedColumns(const QModelIndex &parent, int start, int end)
{
    if( debug_mdl_signals ) qDebug() << "QpTableViewPrivate::_q_updateSpanRemovedColumns( first " << start << ", last " << end<<") nothing,  verticalHeader->length()"<<verticalHeader->length();


    Q_UNUSED(parent)

    //spans.updateRemovedColumns(start, end);
}

void QpTableView::delayed_Repaint()
{
    if (delayed_Repaint_tmr == 0)
        delayed_Repaint_tmr = startTimer(0);
}


QT_END_NAMESPACE

#include "qp_tableview.moc"

#include "moc_qp_tableview.cpp"

#endif // QT_NO_TABLEVIEW
