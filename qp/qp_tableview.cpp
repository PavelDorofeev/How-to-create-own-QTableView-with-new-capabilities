
#include "qp_tableview.h"

#ifndef QT_NO_TABLEVIEW
#include <qp_horheaderview.h>
#include <qp_vertheaderview.h>
#include <qitemdelegate.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qsize.h>
#include <qevent.h>
#include <qbitarray.h>
#include <qscrollbar.h>
#include <qabstractbutton.h>
#include <private/qp_private/qp_tableview_p.h>
#ifndef QT_NO_ACCESSIBILITY
#include <qaccessible.h>
#endif

#include "my_lib/common/dbg.h"

QT_BEGIN_NAMESPACE

const int QpTableView::defaultRowHeight = 60;

const bool QpTableView::debug = false;
const bool QpTableView::debug_paint = false;
const bool QpTableView::debug_init = false;
const bool QpTableView::debug_event = false;
const bool QpTableView::debug_paint_region = false;
const bool QpTableView::debug_paint_row_col = false;
const bool QpTableView::debug_paint_border = false;
const bool QpTableView::debug_selection = false;
const bool QpTableView::debug_scroll = false;
const bool QpTableViewPrivate::debug = false;
const bool QpTableViewPrivate::debug_init = true;
const bool QpTableViewPrivate::debug_selection = false;

const int QpTableView::correct_width_minus_1 = -1;
const int QpTableView::correct_height_minus_1 = -1;

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

void QpTableViewPrivate::init( const Qp_SECTION_TMPL *matrix )
{
    Q_Q(QpTableView);

    if ( debug_init ) qDebug() << "QpTableViewPrivate::init(matrix" << matrix;

    q->setEditTriggers( editTriggers|QAbstractItemView::AnyKeyPressed);

    QpHorHeaderView *horizontal = new QpHorHeaderView( Qt::Horizontal, q);

    horizontal->setClickable(true);
    horizontal->setHighlightSections(true);
    q->setHorizontalHeader(horizontal, matrix);

    QpVertHeaderView *vertical = new QpVertHeaderView( *horizontal, Qt::Vertical, q);
    vertical->setClickable(true);
    vertical->setHighlightSections(true);
    q->setVerticalHeader(vertical);


    tabKeyNavigation = true;

    cornerWidget = new QTableCornerButton2(q);
    cornerWidget->setFocusPolicy(Qt::NoFocus);
    QObject::connect(cornerWidget, SIGNAL(clicked()), q, SLOT(selectAll()));

}

/*!
  \internal
  Trims away indices that are hidden in the treeview due to hidden horizontal or vertical sections.
*/
void QpTableViewPrivate::trimHiddenSelections(QItemSelectionRange *range) const
{
    //if ( debug_selection ) qDebug() << "QpTableViewPrivate::trimHiddenSelections";

    Q_ASSERT(range && range->isValid());

    int top = range->top();
    int left = range->left();
    int bottom = range->bottom();
    int right = range->right();

    while (bottom >= top && verticalHeader->isSectionHidden(bottom))
        --bottom;

    while (right >= left && horizontalHeader->isSectionHidden(right))
        --right;

    if (top > bottom || left > right)
    { // everything is hidden
        *range = QItemSelectionRange();
        return;
    }

    while (verticalHeader->isSectionHidden(top) && top <= bottom)
        ++top;

    while (horizontalHeader->isSectionHidden(left) && left <= right)
        ++left;

    if (top > bottom || left > right)
    { // everything is hidden
        *range = QItemSelectionRange();
        return;
    }

    QModelIndex bottomRight = model->index(bottom, right, range->parent());
    QModelIndex topLeft = model->index(top, left, range->parent());

    *range = QItemSelectionRange(topLeft, bottomRight);
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
        if ( debug_selection ) qDebug() << "QpTableViewPrivate::drawCell hover index:"<<index << " hover " << hover ;

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

    if (index == q->currentIndex())
    {
        const bool focus = (q->hasFocus() || viewport->hasFocus()) && q->currentIndex().isValid();

        if (focus)
            opt.state |= QStyle::State_HasFocus;
    }


    if ( debug_selection ) qDebug() << "QpTableViewPrivate::drawCell index:"<< index.row()<<":" <<index.column() << " opt " << opt ;//<< "\n          painter : " << painter;

    q->style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &opt, painter, q); // what is this?

    q->itemDelegate(index)->paint(painter, opt, index); // this paints text
}


QpTableView::QpTableView( QWidget *parent, Qp_SECTION_TMPL *matrix)
    :
      QAbstractItemView(*new QpTableViewPrivate, parent),
      delayed_Repaint_tmr(0)
{
    Q_D(QpTableView);

    d->init( matrix );

    //viewport()->setBackgroundRole( QPalette::Button );
    //setBackgroundRole( QPalette::Button ); //!!
    if(debug) qDebug() << "ctor QTableView2::QTableView2(QWidget *parent): QAbstractItemView(*new QpTableViewPrivate, parent) backgroundRole:" <<  backgroundRole();

}

QpTableView::QpTableView(QpTableViewPrivate &dd,  Qp_SECTION_TMPL *matrix, QWidget *parent)
    : QAbstractItemView(dd, parent)
{
    Q_D(QpTableView);


    d->init( matrix );

    if( debug ) qDebug() << "ctor QTableView2 (QpTableViewPrivate&, QWidget *) backgroundRole:" << backgroundRole();
    //viewport()->setBackgroundRole( QPalette::Button );
    //setBackgroundRole( QPalette::Button ); //!!

}


QpTableView::~QpTableView()
{
}


void QpTableView::setModel (QAbstractItemModel *model , const Qp_SECTION_TMPL & matrix)
{
    Q_D(QpTableView);


    if (model == d->model)
        return;


    //let's disconnect from the old model
    if (d->model && d->model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        disconnect(d->model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this, SLOT(slot_rowsInserted(QModelIndex,int,int)));

        //                disconnect(d->model, SIGNAL(rowsInserted(QModelIndex,int,int)),
        //                           this, SLOT(_q_updateSpanInsertedRows(QModelIndex,int,int)));
        //                disconnect(d->model, SIGNAL(columnsInserted(QModelIndex,int,int)),
        //                           this, SLOT(_q_updateSpanInsertedColumns(QModelIndex,int,int)));
        //                disconnect(d->model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
        //                           this, SLOT(_q_updateSpanRemovedRows(QModelIndex,int,int)));
        //                disconnect(d->model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
        //                           this, SLOT(_q_updateSpanRemovedColumns(QModelIndex,int,int)));
    }
    if (model)
    { //and connect to the new one
        qDebug() << "QTableView2::setModel";

        connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
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

    d->horizontalHeader->setModel( model , &matrix); // !!


    d->verticalHeader->setModel( model );

    QAbstractItemView::setModel(model);
}

/*!
  \reimp
*/
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

    QAbstractItemView::setRootIndex(index);
}

/*!
  \internal
*/
void QpTableView::doItemsLayout()
{
    Q_D(QpTableView);
    QAbstractItemView::doItemsLayout();

    if( debug_scroll) qDebug() << "QTableView2::doItemsLayout() verticalScrollBar()->value()" << verticalScrollBar()->value();

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
void QpTableView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    qDebug() << "QTableView2::setSelectionModel "<<selectionModel;

    Q_D(QpTableView);

    Q_ASSERT(selectionModel);

    d->verticalHeader->setSelectionModel(selectionModel);

    d->horizontalHeader->setSelectionModel(selectionModel);

    QAbstractItemView::setSelectionModel(selectionModel);
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


void QpTableView::setHorizontalHeader(QpHorHeaderView *header, const Qp_SECTION_TMPL * matrix)
{
    Q_D(QpTableView);

    if (!header || header == d->horizontalHeader)
        return;

    if (d->horizontalHeader && d->horizontalHeader->parent() == this)
        delete d->horizontalHeader;

    d->horizontalHeader = header;
    d->horizontalHeader->setParent(this);

    if ( !d->horizontalHeader->model())
    {
        d->horizontalHeader->setModel( d->model , matrix);

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
        d->verticalHeader->setModel(d->model);

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


void QpTableView::scrollContentsBy(int dx, int dy)
{
    Q_D(QpTableView);

    // dx это шаг в направлении - вправо, + влево

    if( debug_scroll ) qDebug() << "QTableView2::scrollContentsBy dx " << dx << " dy " <<dy;

    d->delayedAutoScroll.stop(); // auto scroll was canceled by the user scrolling

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

/*!
  \reimp
*/
QStyleOptionViewItem QpTableView::viewOptions() const
{
    QStyleOptionViewItem option = QAbstractItemView::viewOptions();

    option.showDecorationSelected = true;

    return option;
}

/*!
    Paints the table on receipt of the given paint event \a event.
*/
void QpTableView::paintEvent(QPaintEvent *event)
{
    Q_D(QpTableView);
    // setup temp variables for the painting

    if( debug ) qDebug()<<"\n---------------------------------------------------------------------------------------------\n                QTableView2::paintEvent \n---------------------------------------------------------------------------------------------\n";

    QStyleOptionViewItemV4 option = d->viewOptionsV4();

    const QPoint offset = d->scrollDelayOffset;

    const bool showGrid = d->showGrid;

    const int gridSize = showGrid ? 1 : 0;

    const int gridHint = style()->styleHint(QStyle::SH_Table_GridLineColor, &option, this);

    const QColor gridColor = static_cast<QRgb>(gridHint);

    const QPen gridPen = QPen(gridColor, 0, d->gridStyle);

    const QpVertHeaderView *verticalHeader = d->verticalHeader;

    const QpHorHeaderView *horizontalHeader = d->horizontalHeader;

    const bool alternate = d->alternatingColors;

    const bool rightToLeft = isRightToLeft();

    QPainter painter(d->viewport);

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
    //    if( debug_paint ) qDebug() << "QTableView2::paintEvent ";

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

    int num_x_cnt =  horizontalHeader->count_of_section() ;

    int sz = num_y_cnt * num_x_cnt;

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
        QPainter painterGrid (d->viewport);

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
                //                  Paint each column item
                // ------------------------------------------------------------

                for (int numX = left_num; numX <= right_num; ++numX)
                {

                    int logicalIndex = horizontalHeader->logicalIndex_atNum_x_line( numX , line );

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
                        //if( debug_paint ) qDebug() << "           currentBit :  " << currentBit << "  field:" << logicalIndex << " EXISTS ALREADY !";

                        continue;
                    }

                    //if( debug_paint )qDebug() << "        currentBit :  " << currentBit << "  field:" << logicalIndex;//<< "  top:" << top; << " section_count:" << section_count ;

                    drawn.setBit( currentBit );

                    int rowh = rowHeight(row) - gridSize; // 99

                    //                    if( numY  == 0  && numX  == 3 )
                    //                        qDebug() << "hgfghghghg";

                    QRect rect =  columnViewportPosition2( logicalIndex );

                    if ( ! rect.isValid() )
                    {
                        qDebug() << "wrong rect for logicalIndex:"<<logicalIndex;
                        continue;
                    }

                    int x1=rect.left();
                    int x2=rect.right();
                    int y1=rect.top();
                    int y2=rect.bottom() ;

                    //if(debug) qDebug() << "                       x1:" << rect.left() << " x2:" << rect.right() << "  y1:" << rect.top() << " y2:" << rect.bottom();

                    rect.setBottom( y2 - gridSize + correct_height_minus_1 );
                    rect.setRight(  x2 - gridSize + correct_width_minus_1 );

                    //if(debug) qDebug() << "                       x1:" << rect.left() << " x2:" << rect.right() << "  y1:" << rect.top() << " y2:" << rect.bottom();

                    rect.moveTop( rowY + y1 );

                    //if(debug) qDebug() << "moveTop                x1:" << rect.left() << " x2:" << rect.right() << "  y1:" << rect.top() << " y2:" << rect.bottom();

                    const QModelIndex index = d->model->index( row, logicalIndex , d->root);


                    if ( ! index.isValid())
                    {
                        qDebug() << "! index.isValid() index " << index <<  d->model->data( index).toString() << "  line " << line << " logicalIndex " << logicalIndex << " numX " << numX;
                        continue;
                    }

                    if( debug_paint_row_col ) qDebug() << "               numX:"  << numX << "  col : "<<logicalIndex;


                    option.rect = rect ; //QRect(colp + (showGrid && rightToLeft ? 1 : 0), row_Y, colw, row_h );

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

                    d->drawCell( &painter, option, index);

                    //-------------------------------------------------------------
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
                        painterGrid.drawLine( xx1 ,  yy2 ,
                                              xx2  , yy2);

                        //                        if( chainVisualIndex == right )
                        //                        {

                        //                            painterGrid.drawLine( x2, y1,
                        //                                                  x2, y2);

                        //                        }

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
QModelIndex QpTableView::indexAt(const QPoint &pos) const
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

    //if( debug ) qDebug() << "   TableView2::indexAt(pos"<<pos<<")  r:"<<row << "  c:"<<col;

    if (row >= 0 && col >= 0)
    {
        QModelIndex idx = d->model->index(row, col, d->root);

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
    \fn QModelIndex QTableView2::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)

    Moves the cursor in accordance with the given \a cursorAction, using the
    information provided by the \a modifiers.

    \sa QAbstractItemView::CursorAction
*/
QModelIndex QpTableView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_D(QpTableView);
    Q_UNUSED(modifiers);

    qDebug() << "QTableView2::moveCursor cursorAction: " <<  cursorAction;

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
    \fn void QTableView2::setSelection(const QRect &rect,
    QItemSelectionModel::SelectionFlags flags)

    Selects the items within the given \a rect and in accordance with
    the specified selection \a flags.
*/
void QpTableView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_D(QpTableView);

    //if( debug_selection ) qDebug() << "QTableView2::setSelection rect" << rect;

    QModelIndex tl = indexAt( QPoint( qMin(rect.left(), rect.right()), qMin(rect.top(), rect.bottom())));


    QModelIndex br = indexAt( QPoint( qMax(rect.left(), rect.right()), qMax(rect.top(), rect.bottom())));

    //if( debug_selection ) qDebug() << "QTableView2::setSelection rect" << rect << "  tl "<< tl << " br " << br;


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

    if( debug_selection ) qDebug()<< " QTableView2::setSelection selection " << selection;

    d->selectionModel->select(selection, command);

}

/*!
    \internal

    Returns the rectangle from the viewport of the items in the given
    \a selection.

    Since 4.7, the returned region only contains rectangles intersecting
    (or included in) the viewport.
*/
QRegion QpTableView::visualRegionForSelection(const QItemSelection &selection) const
{
    if ( debug_selection ) qDebug() << "QTableView2::visualRegionForSelection selection: " << selection ;

    if ( debug_selection )
    {
        foreach (QItemSelectionRange itm, selection)
            if( debug_selection ) qDebug() << "   - " << itm.left() << itm.right() << "   |"<< itm.top() <<itm.bottom();
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

                Now we send each other logical index rectangle
            */


            QModelIndex tl = selectionModel()->model()->index( 0 , range.left() );
            QModelIndex br = selectionModel()->model()->index( 0 , range.right() );

            int rH = rowHeight( range.bottom() );

            for(int ll = range.top(); ll <=range.bottom(); ll++ )
            {
                const int rtop = rowViewportPosition( ll) ;
                const int rBtm = rtop +  rH;

                for(int lgclNum = range.left(); lgclNum <=range.right(); lgclNum++ )
                {

                    QRect  rect = columnViewportPosition2( lgclNum );

                    if( ! rect.isValid())
                    {
                        // it is the skipping number in logical numbers
                        continue;
                    }

                    int rleft = rect.x();

                    int rright = rleft + rect.width() ;


                    const QRect rangeRect( QPoint(rleft, rtop),
                                           QPoint(rright -  gridAdjust + correct_width_minus_1,
                                                  rBtm - gridAdjust + correct_height_minus_1));

                    selectionRegion += rangeRect;

                    if( debug_selection ) qDebug()<< "      rangeRect: " <<rangeRect;
                }


                //if( debug_selection ) qDebug()<< "      rtop: " <<rtop << "  bottom_y "<<bottom_y;

            }
        }
    }

    // resulting region

    if( debug_selection )  qDebug() << "QTableView2::visualRegionForSelection return selectionRegion " << selectionRegion;

    return selectionRegion;
}


QModelIndexList QpTableView::selectedIndexes() const
{
    if( debug_selection ) qDebug() << "QTableView2::selectedIndexes()";

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
    if (newCount < oldCount)
        d->verticalHeader->setUpdatesEnabled(false);
    d->doDelayedItemsLayout();
}

/*!
            This slot is called whenever columns are added or deleted. The
            previous number of columns is specified by \a oldCount, and the new
            number of columns is specified by \a newCount.
        */
void QpTableView::columnCountChanged(int, int)
{
    Q_D(QpTableView);
    updateGeometries();
    if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem)
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

    QRect vg = d->viewport->geometry();

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

    //if( debug ) qDebug() << "QTableView2::updateGeometries";

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

        if( debug_scroll ) qDebug() << " QTableView2::updateGeometries() ";
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
int QpTableView::sizeHintForRow(int row) const
{
    Q_D(const QpTableView);

    int rowH = d->horizontalHeader->row_height();

    if( rowH == -1)
        return QpTableView::defaultRowHeight;

    return d->showGrid ? rowH + 1 : rowH;

}

/*!
            Returns the size hint for the given \a column's width or -1 if
            there is no model.

            If you need to set the width of a given column to a fixed value, call
            QpHorHeaderView::resizeSection() on the table's horizontal header.

            If you reimplement this function in a subclass, note that the value you
            return will be used when resizeColumnToContents() or
            QpHorHeaderView::resizeSections() is called. If a larger column width is
            required by either the horizontal header or the item delegate, the larger
            width will be used instead.

            \sa QWidget::sizeHint, horizontalHeader()
        */
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
    if( debug ) qDebug()<< " QTableView2::sizeHintForColumn(column:"<<column<<")" <<  hint;
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

    //if(debug) qDebug() << "QTableView2::rowViewportPosition y:" << y;

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

    int row = d->verticalHeader->logicalIndexAt( y ); // QTableView2::lines;

    //if(debug) qDebug() << "QTableView2::rowAt(y:"<<y<<") = " << row;

    return row ;
}

/*!
            \since 4.1

            Sets the height of the given \a row to be \a height.
        */
void QpTableView::setRowHeight(int row, int height)
{
    Q_D(const QpTableView);
    d->verticalHeader->resizeSection(row, height);
}

/*!
            Returns the height of the given \a row.

            \sa resizeRowToContents(), columnWidth()
        */
int QpTableView::rowHeight(int row) const
{
    Q_D(const QpTableView);

    return d->verticalHeader->sectionSize(row);
}

//int QTableView2::rowLine(int y) const
//{
//    Q_D(const QTableView2);

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

/*!
            Returns the column in which the given x-coordinate, \a x, in contents
            coordinates is located.

            \note This function returns -1 if the given coordinate is not valid
            (has no column).

            \sa rowAt()
        */
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
void QpTableView::setColumnWidth(int column, int width)
{
    Q_D(const QpTableView);
    d->horizontalHeader->resizeSection(column, width);
}

/*!
            Returns the width of the given \a column.

            \sa resizeColumnToContents(), rowHeight()
        */
int QpTableView::columnWidth(int column) const
{
    Q_D(const QpTableView);

    return d->horizontalHeader->sectionSize(column);
}

/*!
            Returns true if the given \a row is hidden; otherwise returns false.

            \sa isColumnHidden()
        */
bool QpTableView::isRowHidden(int row) const
{
    Q_D(const QpTableView);
    return d->verticalHeader->isSectionHidden(row);
}

/*!
            If \a hide is true \a row will be hidden, otherwise it will be shown.

            \sa setColumnHidden()
        */
void QpTableView::setRowHidden(int row, bool hide)
{
    Q_D(QpTableView);
    if (row < 0 || row >= d->verticalHeader->count())
        return;
    d->verticalHeader->setSectionHidden(row, hide);
}

/*!
            Returns true if the given \a column is hidden; otherwise returns false.

            \sa isRowHidden()
        */
bool QpTableView::isColumnHidden(int column) const
{
    Q_D(const QpTableView);
    return d->horizontalHeader->isSectionHidden(column);
}

/*!
          If \a hide is true the given \a column will be hidden; otherwise it
          will be shown.

          \sa setRowHidden()
        */
void QpTableView::setColumnHidden(int column, bool hide)
{
    Q_D(QpTableView);
    if (column < 0 || column >= d->horizontalHeader->count())
        return;
    d->horizontalHeader->setSectionHidden(column, hide);
}

/*!
            \since 4.2
            \property QTableView2::sortingEnabled
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

bool QpTableView::isSortingEnabled() const
{
    Q_D(const QpTableView);
    return d->sortingEnabled;
}

/*!
            \property QTableView2::showGrid
            \brief whether the grid is shown

            If this property is true a grid is drawn for the table; if the
            property is false, no grid is drawn. The default value is true.
        */
bool QpTableView::showGrid() const
{
    Q_D(const QpTableView);

    return d->showGrid;
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
          \property QTableView2::gridStyle
          \brief  the pen style used to draw the grid.

          This property holds the style used when drawing the grid (see \l{showGrid}).
        */
Qt::PenStyle QpTableView::gridStyle() const
{
    Q_D(const QpTableView);

    return d->gridStyle;
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

/*!
            \property QTableView2::wordWrap
            \brief the item text word-wrapping policy
            \since 4.3

            If this property is true then the item text is wrapped where
            necessary at word-breaks; otherwise it is not wrapped at all.
            This property is true by default.

            Note that even of wrapping is enabled, the cell will not be
            expanded to fit all text. Ellipsis will be inserted according to
            the current \l{QAbstractItemView::}{textElideMode}.

        */
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

/*!
            \property QTableView2::cornerButtonEnabled
            \brief whether the button in the top-left corner is enabled
            \since 4.3

            If this property is true then button in the top-left corner
            of the table view is enabled. Clicking on this button will
            select all the cells in the table view.

            This property is true by default.
        */
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

    //if( debug ) qDebug() << "QTableView2::visualRect rect " << rect << " row_viewpport_y " << row_viewpport_y << "  offset_y " << offset_y;

    rect.moveTop( offset_y );

    //int colw = columnWidth(index.column());

    const int i = showGrid() ? 1 : 0;

    //if( debug ) qDebug() << "QTableView2::visualRect return rect " << rect;

    return rect;
}

/*!
            \internal

            Makes sure that the given \a item is visible in the table view,
            scrolling if necessary.
        */
void QpTableView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_D(QpTableView);

    if( debug_scroll ) qDebug() << "QTableView2::scrollTo " << index << " ScrollHint hint " << hint;

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
void QpTableView::rowResized(int row, int oldHeight, int newHeight)
{
    Q_D(QpTableView);
    d->rowsToUpdate.append(row);

    if (d->rowResizeTimerID == 0)
        d->rowResizeTimerID = startTimer(0);
}

void QpTableView::columnResized_Y()
{
    Q_D(QpTableView);

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

void QpTableView::columnResized(int column, int oldWidth, int newWidth)
{
    Q_D(QpTableView);

    //d->horizontalHeader->resizeSection( column );

    d->columnsToUpdate.append(column);

    if (d->columnResizeTimerID == 0)
        d->columnResizeTimerID = startTimer(0);
}


void QpTableView::timerEvent(QTimerEvent *event)
{
    Q_D(QpTableView);

    // -----------------------------------------------------------------------
    // This timer is only for repainting with mouse changes section sizes
    // -----------------------------------------------------------------------

    if (event->timerId() == d->columnResizeTimerID)
    {
        if( debug_event ) qDebug() << "QTableView2::timerEvent columnResizeTimerID";

        updateGeometries();

        killTimer(d->columnResizeTimerID);

        d->columnResizeTimerID = 0;

        QRect rect;
        int viewportHeight = d->viewport->height();
        int viewportWidth = d->viewport->width();


        for (int i = d->columnsToUpdate.size()-1; i >= 0; --i)
        {
            int column = d->columnsToUpdate.at(i);

            int x = horizontalHeader()->left_common_border_x( column) ;

            rect |= QRect(x, 0, viewportWidth - x, viewportHeight);
        }


        d->viewport->update(rect.normalized());
        d->columnsToUpdate.clear();
    }

    if (event->timerId() == d->rowResizeTimerID)
    {
        if( debug_event ) qDebug() << "QTableView2::timerEvent rowResizeTimerID";

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
        if( debug_event ) qDebug() << "QTableView2::timerEvent delayed_Repaint_tmr";

        updateGeometries();
        killTimer( delayed_Repaint_tmr);
        delayed_Repaint_tmr = 0;

        d->horizontalHeader->viewport()->updateGeometry();
        d->horizontalHeader->viewport()->update();

        d->verticalHeader->viewport()->updateGeometry();
        d->verticalHeader->viewport()->update();
        d->viewport->update();

    }

    QAbstractItemView::timerEvent(event);
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

    //    if (d->hasSpans())
    //    {
    //        d->viewport->update();
    //    }
    //    else
    //    {
    int oldLeft = columnViewportPosition(logicalOldIndex);
    int newLeft = columnViewportPosition(logicalNewIndex);
    int oldRight = oldLeft + columnWidth(logicalOldIndex);
    int newRight = newLeft + columnWidth(logicalNewIndex);
    int left = qMin(oldLeft, newLeft);
    int right = qMax(oldRight, newRight);
    int width = right - left;
    d->viewport->update(left, 0, width, d->viewport->height());
    //    }
}

/*!
            Selects the given \a row in the table view if the current
            SelectionMode and SelectionBehavior allows rows to be selected.

            \sa selectColumn()
        */
void QpTableView::selectRow(int row)
{
    Q_D(QpTableView);

    //horizontalHeader()->clearSelection(); //!!

    d->selectRow(row, true);
}

/*!
            Selects the given \a column in the table view if the current
            SelectionMode and SelectionBehavior allows columns to be selected.

            \sa selectRow()
        */
void QpTableView::selectColumn(int column)
{
    Q_D(QpTableView);
    d->selectColumn(column, true);
}

/*!
            Hide the given \a row.

            \sa showRow() hideColumn()
        */
void QpTableView::hideRow(int row)
{
    Q_D(QpTableView);
    d->verticalHeader->hideSection(row);
}

/*!
            Hide the given \a column.

            \sa showColumn() hideRow()
        */
void QpTableView::hideColumn(int column)
{
    Q_D(QpTableView);
    d->horizontalHeader->hideSection(column);
}

/*!
            Show the given \a row.

            \sa hideRow() showColumn()
        */
void QpTableView::showRow(int row)
{
    Q_D(QpTableView);
    d->verticalHeader->showSection(row);
}

/*!
            Show the given \a column.

            \sa hideColumn() showRow()
        */
void QpTableView::showColumn(int column)
{
    Q_D(QpTableView);
    d->horizontalHeader->showSection(column);
}

/*!
            Resizes the given \a row based on the size hints of the delegate
            used to render each item in the row.
        */
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

/*!
            Resizes the given \a column based on the size hints of the delegate
            used to render each item in the column.

            \note Only visible columns will be resized. Reimplement sizeHintForColumn()
            to resize hidden columns as well.
        */
void QpTableView::resizeColumnToContents(int column)
{
    Q_D(QpTableView);

    int content = sizeHintForColumn(column);

    int header = d->horizontalHeader->sectionSizeHint(column);

    d->horizontalHeader->resizeSection(column, qMax(content, header));
}

/*!
            Resizes all columns based on the size hints of the delegate
            used to render each item in the columns.
        */
void QpTableView::resizeColumnsToContents()
{
    Q_D(QpTableView);
    d->horizontalHeader->resizeSections(QpHorHeaderView::ResizeToContents);
}

/*!
          \obsolete
          \overload

          Sorts the model by the values in the given \a column.
        */
void QpTableView::sortByColumn(int column)
{
    Q_D(QpTableView);
    if (column == -1)
        return;
    d->model->sort(column, d->horizontalHeader->sortIndicatorOrder());
}

/*!
          \since 4.2

          Sorts the model by the values in the given \a column in the given \a order.

          \sa sortingEnabled
         */
void QpTableView::sortByColumn(int column, Qt::SortOrder order)
{
    Q_D(QpTableView);
    d->horizontalHeader->setSortIndicator(column, order);
    sortByColumn(column);
}

/*!
            \internal
        */
void QpTableView::verticalScrollbarAction(int action)
{
    if( debug_scroll) qDebug() << "QTableView2::verticalScrollbarAction";

    QAbstractItemView::verticalScrollbarAction(action); // NOTHING
}

/*!
            \internal
        */
void QpTableView::horizontalScrollbarAction(int action)
{
    if( debug_scroll) qDebug() << "QTableView2::horizontalScrollbarAction " << action;

    QAbstractItemView::horizontalScrollbarAction(action);
}

/*!
          \reimp
        */
bool QpTableView::isIndexHidden(const QModelIndex &index) const
{
    Q_D(const QpTableView);
    Q_ASSERT(d->isIndexValid(index));
    if (isRowHidden(index.row()) || isColumnHidden(index.column()))
        return true;
    //    if (d->hasSpans()) {
    //        QSpanCollection2::Span span = d->span(index.row(), index.column());
    //        return !((span.top() == index.row()) && (span.left() == index.column()));
    //    }
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

    if (row >= 0 && row < model->rowCount(root))
    {
        int column = horizontalHeader->logicalIndexAt( QPoint( 0 , 0 ) ); //??????

        QModelIndex index = model->index(row, column, root);

        QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);
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

        int firstLgclIdx = horizontalHeader->leftTopVisualNumX();

        //QModelIndex tl = model->index(qMin(rowSectionAnchor, row), firstLgclIdx , root);
        QModelIndex tl = model->index( row , firstLgclIdx , root);

        int lastLgclIdx = horizontalHeader->lastLogicalNum();
        //int lastLgclIdx = horizontalHeader->rightBtmLogicalNum();

        QModelIndex br = model->index(qMax(rowSectionAnchor, row), lastLgclIdx , root); //!!

        if (verticalHeader->sectionsMoved() && tl.row() != br.row())
            q->setSelection(q->visualRect(tl)|q->visualRect(br), command);
        else
            selectionModel->select(QItemSelection(tl, br), command);
    }
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
    //qDebug() << "QTableView2::currentChanged not debugging current "<< current << "  previous "<<previous;

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


void QpTableView::selectionChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
{
    if ( debug_selection )
    {
        qDebug() << "QTableView2::selectionChanged selected:" << selected;
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

    QAbstractItemView::selectionChanged(selected, deselected);
}

//void QTableView2::update()
//{
//    qDebug() << "QTableView2::update";

//    QWidget::update();
//}

//void QTableView2::update(const QRect &rect)
//{
//    qDebug() << "QTableView2::update(rect" << rect <<")";

//    QWidget::update( rect );
//}

void QpTableView::updateGeometry()
{
    qDebug() << "QTableView2::updateGeometry";

    QWidget::updateGeometry();
}

int QpTableView::visualIndex(const QModelIndex &index) const
{
    return index.row();
}

bool QpTableView::init_template( const Qp_SECTION_TMPL &matrix )
{
    horizontalScrollBar()->setValue( 0 ); //!!

    clearSelection();


    if( horizontalHeader()->init_sections_template( matrix ) )
    {
        updateGeometry(); //!!
        delayed_Repaint();//!!
        return true;
    }

    return false;
}

void QpTableView::slot_rowsInserted(const QModelIndex &parent, int first, int last)
{
    if( debug ) qDebug() << "QTableView2::slot_rowsInserted first " << first << " last " <<last;
    //qDebug() << "    rowHeight   " << rowHeight( );

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
