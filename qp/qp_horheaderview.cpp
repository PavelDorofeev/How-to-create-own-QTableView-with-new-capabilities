#include "qp_horheaderview.h"

#ifndef QT_NO_ITEMVIEWS
#include <qbitarray.h>
#include <qbrush.h>
#include <qdebug.h>
#include <qevent.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qvector.h>
#include <qapplication.h>
#include <qvarlengtharray.h>
#include <qabstractitemdelegate.h>
#include <qvariant.h>
#include <private/qp_private/qp_horheaderview_p.h>
#include <private/qabstractitemmodel_p.h>
#include <QpTableView>
#include "my_lib/common/dbg.h"

#ifndef QT_NO_DATASTREAM
#include <qdatastream.h>
#endif

QT_BEGIN_NAMESPACE


const bool QpHorHeaderView::debug = false;
const bool QpHorHeaderView::debug_paint = false;
const bool QpHorHeaderView::debug_selection = false;
const bool QpHorHeaderView::debug_scroll = false;
const bool QpHorHeaderView::debug_resize = false;
const bool QpHorHeaderView::debug_init = true;
const int QpHorHeaderView::default_section_width = 150;

const bool QpHorHeaderViewPrivate::debug = false;
const bool QpHorHeaderViewPrivate::debug_resize = false;

const int qp::CELL_NODES::NODE_UNDEFINED  = -1;

QpHorHeaderView::QpHorHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QAbstractItemView(*new QpHorHeaderViewPrivate, parent)

{
    Q_D(QpHorHeaderView);
    d->setDefaultValues(orientation);
    initialize();
}

/*!
  \internal
*/
QpHorHeaderView::QpHorHeaderView(QpHorHeaderViewPrivate &dd,
                                 Qt::Orientation orientation, QWidget *parent)
    : QAbstractItemView(dd, parent)
{
    Q_D(QpHorHeaderView);
    d->setDefaultValues(orientation);
    initialize();
}

/*!
  Destroys the header.
*/

QpHorHeaderView::~QpHorHeaderView()
{
}

/*!
  \internal
*/
void QpHorHeaderView::initialize()
{
    Q_D(QpHorHeaderView);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(NoFrame);
    setFocusPolicy(Qt::NoFocus);

    d->viewport->setMouseTracking(true);
    d->viewport->setBackgroundRole(QPalette::Button);
    d->textElideMode = Qt::ElideNone;

    delete d->itemDelegate;

}

void QpHorHeaderView::setModel(QAbstractItemModel *model , const Qp_SECTION_TMPL * matrix)
{
    Q_D(QpHorHeaderView);

    if (model == this->model())
        return;


    if (d->model && d->model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        QObject::disconnect(d->model, SIGNAL(columnsInserted(QModelIndex,int,int)),
                            this, SLOT(sectionsInserted(QModelIndex,int,int)));

        QObject::disconnect(d->model, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
                            this, SLOT(sectionsAboutToBeRemoved(QModelIndex,int,int)));

        QObject::disconnect(d->model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
                            this, SLOT(_q_sectionsRemoved(QModelIndex,int,int)));

        QObject::disconnect(d->model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
                            this, SLOT(headerDataChanged(Qt::Orientation,int,int)));

        QObject::disconnect(d->model, SIGNAL(layoutAboutToBeChanged()),
                            this, SLOT(_q_layoutAboutToBeChanged()));
    }

    if (model && model != QAbstractItemModelPrivate::staticEmptyModel())
    {
        QObject::connect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),
                         this, SLOT(sectionsInserted(QModelIndex,int,int)));

        QObject::connect(model, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
                         this, SLOT(sectionsAboutToBeRemoved(QModelIndex,int,int)));

        QObject::connect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)),
                         this, SLOT(_q_sectionsRemoved(QModelIndex,int,int)));

        QObject::connect(model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
                         this, SLOT(headerDataChanged(Qt::Orientation,int,int)));
        QObject::connect(model, SIGNAL(layoutAboutToBeChanged()),
                         this, SLOT(_q_layoutAboutToBeChanged()));
    }

    d->state = QpHorHeaderViewPrivate::NoClear;

    QAbstractItemView::setModel(model);

    d->state = QpHorHeaderViewPrivate::NoState;

    // Users want to set sizes and modes before the widget is shown.
    // Thus, we have to initialize when the model is set,
    // and not lazily like we do in the other views.

    if( matrix != 0)
        init_sections_template( *matrix );

    initializeSections();
}

/*!
    Returns the orientation of the header.

    \sa Qt::Orientation
*/

Qt::Orientation QpHorHeaderView::orientation() const
{
    Q_D(const QpHorHeaderView);

    return Qt::Horizontal;
}

/*!
    Returns the offset of the header: this is the header's left-most (or
    top-most for vertical headers) visible pixel.

    \sa setOffset()
*/

int QpHorHeaderView::offset() const
{
    Q_D(const QpHorHeaderView);
    return d->offset;
}

/*!
    \fn void QpHorHeaderView::setOffset(int offset)

    Sets the header's offset to \a offset.

    \sa offset(), length()
*/

void QpHorHeaderView::setOffset(int newOffset)
{
    if( debug_scroll) qDebug() << "QpHorHeaderView::setOffset( " <<newOffset << ")";

    Q_D(QpHorHeaderView);

    if (d->offset == (int)newOffset)
        return;

    int ndelta = d->offset - newOffset;

    d->offset = newOffset;

    if ( debug_scroll ) qDebug() << "   scroll  ndelta: " << ndelta <<" isRightToLeft() " << isRightToLeft();

    d->viewport->scroll( ndelta , 0);

    if (d->state == QpHorHeaderViewPrivate::ResizeSection_X)
    {
        QPoint cursorPos = QCursor::pos();

        if( debug_scroll ) qDebug() << "QCursor::setPos";

        QCursor::setPos( cursorPos.x() + ndelta, cursorPos.y());

        d->firstPos_x += ndelta;
        d->lastPos_x += ndelta;
    }
}



int QpHorHeaderView::logicalIndexAt(int x, int line) const
{
    Q_D(const QpHorHeaderView);

    // x - coordinate in viewport

    int visualIdx = d->headerVisualIndexAt2 ( x, line);

    int logicalIdx = logicalIndex ( visualIdx );

    return  logicalIdx;

    //return logicalIndexAt(ax , line) ; // !!
}

int QpHorHeaderView::logicalIndexAt(const QPoint &pos) const
{
    // pos - coordinate in viewport

    //Q_D(QpHorHeaderView);

    int y = pos.y();

    //    int visual_incomplete_first_section = y % d->row_height(); // we navigate for last section
    //    if(  viewport >0 )


    const int line = get_section_line( y );

    if(line <0 )
        return -1;

    int xx = pos.x();

    const int logicalIdx = logicalIndexAt( xx , line );

    return logicalIdx;
}

void QpHorHeaderView::setOffsetToSectionPosition(int xNum)
{
    Q_D(QpHorHeaderView);


    if (xNum > -1 && xNum < d->offsets_x.count() - 1)
    {
        //int adj = d->adjustedVisualIndex( xNum );

        setOffset( d->offsets_x[ xNum ] );

        //        int xPosition = d->headerSectionPosition2( xNum ).x();

        //        if( debug ) qDebug() << "QpHorHeaderView::setOffsetToSectionPosition visualIndex "<<xNum << "  position " << xPosition;

        //        setOffset( xPosition );
    }
}

void QpHorHeaderView::setOffsetToLastSection()
{
    Q_D(const QpHorHeaderView);

    int size =  viewport()->width() ;
    int position = length() - size;
    setOffset(position);
}


int QpHorHeaderView::length() const
{
    Q_D(const QpHorHeaderView);
    //Q_ASSERT(d->headerLength() == d->length);

    int count = d->offsets_x.count() ;

    if(count == 0 )
        return -1;

    //qDebug() << "QpHorHeaderView::length() " << d->offsets_x.at( count - 1) ;

    return d->offsets_x.at( count - 1);
}


QSize QpHorHeaderView::sizeHint() const
{
    Q_D(const QpHorHeaderView);

    if (d->cachedSizeHint.isValid())

        return d->cachedSizeHint;

    d->cachedSizeHint = QSize(0, 0); //reinitialize the cached size hint

    if( d->offsets_y.count()<=0 )

        return QSize ( 60 , 60 );


    int y_max  = d->offsets_y[ d->offsets_y.count() -1 ];

    int width_max=0;

    for ( int col = 1; col < d->offsets_x.count(); col++ )
    {
        if( col == 1 )
        {
            width_max = d->offsets_x[ col ];
            continue;
        }
        int width = d->offsets_x[ col ] - d->offsets_x[ col -1 ];

        width_max = qMax( width_max , width );

    }

    d->cachedSizeHint = QSize( width_max , y_max );

    //    const int sectionCount = count();

    //    // get size hint for the first n sections
    //    int i = 0;

    //    for (int checked = 0; checked < 100 && i < sectionCount; ++i)
    //    {
    //        if (isSectionHidden(i))
    //            continue;

    //        checked++;

    //        QSize hint = sectionSizeFromContents( i );

    //        hint.setHeight( 100 );

    ////        QSize sz( 60 , 60 );
    ////        hint = sz;

    //        d->cachedSizeHint = d->cachedSizeHint.expandedTo(hint);
    //    }
    //    // get size hint for the last n sections
    //    i = qMax(i, sectionCount - 100 );

    //    for (int j = sectionCount - 1, checked = 0; j >= i && checked < 100; --j)
    //    {
    //        if (isSectionHidden(j))
    //            continue;

    //        checked++;
    //        QSize hint = sectionSizeFromContents(j);

    //        d->cachedSizeHint = d->cachedSizeHint.expandedTo(hint);
    //    }

    if ( debug )  qDebug() << " QpHorHeaderView::sizeHint() d->cachedSizeHint " << d->cachedSizeHint;

    return d->cachedSizeHint;
}


int QpHorHeaderView::sectionSizeHint(int logicalIndex) const
{
    Q_D(const QpHorHeaderView);

    if (isSectionHidden(logicalIndex))
        return 0;

    if (logicalIndex < 0 || logicalIndex >= count())
        return -1;

    QSize size;
    QVariant value = d->model->headerData(logicalIndex, Qt::Horizontal, Qt::SizeHintRole);

    if (value.isValid())
        size = qvariant_cast<QSize>(value);
    else
        size = sectionSizeFromContents(logicalIndex);

    int hint = size.width() ;

    if( debug )        qDebug() << "QpHorHeaderView::sectionSizeHint size: " << size << "  hint " << hint;

    return qMax(minimumSectionSize(), hint);
}


int QpHorHeaderView::visual_xNum_At( int x ) const
{
    Q_D(const QpHorHeaderView);

    int x_num = -1;

    //qDebug() << "QpHorHeaderView::visual_xNum_At d->offset " << d->offset << " x " <<x;

    x = x + d->offset; // !!

    //qDebug() << "QpHorHeaderView::visual_xNum_At d->offset " << d->offset << " x " <<x;

    for( int ii=0; ii < d->offsets_x.count() -1 ; ii++)
    {
        if( x < d->offsets_x [ ii + 1 ])
        {
            x_num = ii ;
            break;
        }
    }

    return x_num;
}

int QpHorHeaderView::xNum_count( ) const
{
    Q_D(const QpHorHeaderView);

    return d->offsets_x.count() -1;
}


int QpHorHeaderView::visualIndexAt(int x, int y) const
{
    Q_D(const QpHorHeaderView);

    // x - coordinate in viewport

    int vposition = x;

    d->executePostedLayout();

    d->executePostedResize();

    const int count = d->sectionCount;

    //if (debug)        qDebug() << "QpHorHeaderView::visualIndexAt x " << x <<" y " << y;

    if (count < 1)
    {
        if (debug)           qDebug() << "QpHorHeaderView::visualIndexAt d->sectionCount < 0 position x " << x << " y " << y << " visual = -1";
        return -1;
    }

    //vposition += d->offset;

    //qDebug() << "        vposition " << vposition <<" d->offset" << d->offset;

    if (vposition > d->length_x) // if out of table area
    {
        return -1;
    }

    int line = get_section_line( y );

    if( line == -1)
        return -1;

    int visual = d->headerVisualIndexAt2( vposition , line );

    if (visual < 0)
    {

        return -1;
    }

    //    while ( d->isVisualIndexHidden( visual ) )
    //    {
    //        ++visual;

    //        if (visual >= count)
    //        {
    //            return -1;
    //        }
    //    }

    //if( debug )        qDebug() << "QpHorHeaderView::visualIndexAt position x " << x << " y " << y <<  " line " << line << " visual = " << visual;

    return visual;
}

int QpHorHeaderView::visualIndexAt_end(int x) const
{
    Q_D(const QpHorHeaderView);

    int vposition = x;

    d->executePostedLayout();

    d->executePostedResize();

    const int count = d->sectionCount;

    //if( debug ) qDebug() << "QpHorHeaderView::visualIndexAt_max x " << x ;

    if (count < 1)
    {
        //if( debug ) qDebug() << "QpHorHeaderView::visualIndexAt_max d->sectionCount < 0 position x " << x << " visual = -1";
        return -1;
    }

    vposition += d->offset;


    if (vposition > d->length_x) // if out of table area
    {
        //if( debug ) qDebug() << "QpHorHeaderView::visualIndexAt_max [ vposition > d->length]  vposition "<< vposition<< "  d->length " << d->length  <<"   position x " << x << " visual = " << -1;

        return -1;
    }

    int visual_max_idx_into_section = -1;


    for( int line=0; line < d->offsets_y.count() - 1; line++)
    {
        visual_max_idx_into_section = qMax( visual_max_idx_into_section , d->headerVisualIndexAt2( vposition , line )) ;
    }

    if (visual_max_idx_into_section < 0)
    {
        if( debug )            qDebug() << "QpHorHeaderView::visualIndexAt_max [visual < 0] position x " << x << " visual = " << visual_max_idx_into_section;
        return -1;
    }


    return visual_max_idx_into_section;
}


/*!
    Returns the section that covers the given \a position in the viewport.

    \sa visualIndexAt(), isSectionHidden()
*/

//int QpHorHeaderView::logicalIndexAt(int position) const
//{
//    if( debug )
//        qDebug() << (orientation() == Qt::Vertical ? "Vertical":"Horizontal") <<"  QpHorHeaderView::logicalIndexAt position x: " << position;

//    int visual = visualIndexAt( position , 1 );

//    if( orientation() == Qt::Vertical)
//    {
//        visual = visual / QTableView2::lines;
//    }
//    else if( orientation() == Qt::Horizontal)
//    {

//    }

//    if (visual > -1)
//    {
//        int lgcIndex = logicalIndex(visual);

//        return lgcIndex;
//    }

//    return -1;
//}

/*!
    Returns the width (or height for vertical headers) of the given
    \a logicalIndex.

    \sa length(), setResizeMode(), defaultSectionSize()
*/

const qp::CELL_NODES QpHorHeaderView::get_logicalIdex_nodes(int logicalIndex) const
{
    Q_D(const QpHorHeaderView);

    if( ! d->map.contains( logicalIndex))

        return qp::CELL_NODES();

    return d->map[ logicalIndex ];
}

int QpHorHeaderView::get_section_line( int y ) const
{
    // y - this is coordinate into viewport

    Q_D(const QpHorHeaderView);


    int count = d->offsets_y.count() - 1;

    if( count == 0)
        return -1; // visual matrix is not initialized a while

    int last = count ;

    int rowHeight = d->offsets_y.at( last );

    int viewport_y = d->viewport->y();

    int len = d->length_x;

    int offset_Y =  y % rowHeight ; // 35 | 46

    int ll = 0;
    int line = -1;

    for (line = 0; line < d->offsets_y.count() -1; line++ )
    {
        if( offset_Y < d->offsets_y.at( line + 1))
        {
            break;
        }
    }

    return line;

}

int QpHorHeaderView::get_section_num( int x ) const
{
    // x - this is coordinate into viewport

    Q_D(const QpHorHeaderView);

    int count = d->offsets_x.count() - 1;

    if( count == 0)
        return -1; // visual matrix is not initialized a while

    int viewport_x = d->viewport->x();

    int offset_X = x - viewport_x;

    //int offset_Y =  y % rowHeight ; // 35 | 46

    int col = -1;

    for (col = 0; col < d->offsets_x.count() - 1; col++ )
    {
        if( offset_X < d->offsets_x.at( col + 1))
        {
            break;
        }
    }

    return col;

}

int QpHorHeaderView::xNum_size( int xNum ) const
{
    Q_D(const QpHorHeaderView);

    //    int left;

    //    if( col == d->offsets_x_.count() - 1 )
    //        left = 0;
    //    else
    //        left = d->offsets_x.at( col );

    if( xNum < 0 || xNum > d->offsets_x.count()-2 )
    {
        qDebug() <<  d->offsets_x;
        return 0;
    }

    int width = d->offsets_x[ xNum + 1 ] - d->offsets_x[ xNum ];

    return width;

}

int QpHorHeaderView::sectionSize(int logicalIndex) const
{
    Q_D(const QpHorHeaderView);

    if (isSectionHidden(logicalIndex))
        return 0;

    if (logicalIndex < 0 || logicalIndex >= count())
        return 0;

    int visual = visualIndex(logicalIndex); // ??

    if (visual == -1)
        return 0;

    d->executePostedResize();

    int headerSectionSize = d->headerSectionSize(visual);

    //int headerSectionSize = d->offsets_x_[ visual + 1 ];


    //if( debug )  qDebug() << "  QpHorHeaderView::sectionSize(logicalIndex:"<<logicalIndex<<")="<<headerSectionSize << "  visual:"<<visual;

    return  headerSectionSize ;
}

/*!

    Returns the section position of the given \a logicalIndex, or -1
    if the section is hidden. The position is measured in pixels from
    the first visible item's top-left corner to the top-left corner of
    the item with \a logicalIndex. The measurement is along the x-axis
    for horizontal headers and along the y-axis for vertical headers.

    \sa sectionViewportPosition()
*/

const QRect QpHorHeaderView::sectionPosition2(int logicalIndex) const
{
    Q_D( const QpHorHeaderView);

    int visual = visualIndex( logicalIndex );
    // in some cases users may change the selections
    // before we have a chance to do the layout


    if (visual == -1)
        return QRect();

    d->executePostedResize();


    const QRect headerSectionPosition = d->headerSectionPosition2( visual );

    //    qDebug() << "  QpHorHeaderView::sectionPosition(logicalIndex:"<<logicalIndex<<")"
    //             <<  " visual: "<<visual
    //              << "  headerSectionPosition:" << headerSectionPosition;

    return headerSectionPosition;
}

//int QpHorHeaderView::sectionPosition(int logicalIndex) const
//{
//    Q_D(const QpHorHeaderView);

//    int visual = visualIndex( logicalIndex );
//    // in some cases users may change the selections
//    // before we have a chance to do the layout

//    //    if (visual == 3)
//    //        qDebug() << "ASDSADSA";

//    if (visual == -1)
//        return -1;

//    d->executePostedResize();

//    int headerSectionPosition = d->headerSectionPosition2( visual ).y();

//    //    qDebug() << (orientation() == Qt::Vertical ? "Vertical":"Horizontal")
//    //             << "  QpHorHeaderView::sectionPosition(logicalIndex:"<<logicalIndex<<")"
//    //             <<  " visual: "<<visual
//    //              << "  headerSectionPosition:" << headerSectionPosition;

//    return headerSectionPosition;
//}

/*!
    Returns the section viewport position of the given \a logicalIndex.

    If the section is hidden, the return value is undefined.

    \sa sectionPosition(), isSectionHidden()
*/

QRect QpHorHeaderView::sectionViewportPosition2( int logicalIndex ) const
{
    Q_D(const QpHorHeaderView);

    if ( ! d->map.contains( logicalIndex) )
        return QRect();

    QRect rect = sectionPosition2( logicalIndex );

    if (  ! rect.isValid())
        return rect;


    if ( d->offset !=0 )
    {
        int x = rect.x() -  d->offset;

        rect.moveLeft( x  );

    }


    //    qDebug() << (orientation() == Qt::Vertical ? "Vertical":"Horizontal")  << " QpHorHeaderView::sectionViewportPosition logicalIndex " << logicalIndex << "  offsetPosition " << offsetPosition << "  position " << position;

    return rect;
}
int QpHorHeaderView::leftTopVisualNumX( ) const
{
    Q_D(const QpHorHeaderView);

    if ( d->offsets_x.count() <=0)
        return -1;

    if ( d->offsets_y.count() <=0)
        return -1;

    int lastLgcl = d->visual_matrix[ 0 ] [ 0 ];

    return lastLgcl;
}


int QpHorHeaderView::rightBtmLogicalNum( ) const
{
    Q_D(const QpHorHeaderView);

    if ( d->offsets_x.count() <= 1)
        return -1;

    if ( d->offsets_y.count() <= 1)
        return -1;

    int line = d->offsets_y.count() - 2;
    int last_NumX =  d->offsets_x.count() - 2;
    int br_lgcl = d->visual_matrix [ line ] [ last_NumX ];

    return br_lgcl;
}

int QpHorHeaderView::lastLogicalNum( ) const
{
    Q_D(const QpHorHeaderView);

    if ( d->map.count() <= 0)
        return -1;

    // we maybe not have sequental numbers , i.e. 0,2,3,4,5 ( 1 is not presents)

    QList<int> lst = d->map.keys();


    QHashIterator<int , qp::CELL_NODES > ii(d->map);

    int max_key=-1;

    while( ii.hasNext() )
    {
        ii.next();
        max_key = qMax( max_key , ii.key());
    }


    return max_key;
}


void QpHorHeaderView::moveSection(int from, int to)
{
    Q_D(QpHorHeaderView);

    if( debug )   qDebug()<<"QpHorHeaderView::moveSection";

    //    d->executePostedLayout();
    //    if (from < 0 || from >= d->sectionCount || to < 0 || to >= d->sectionCount)
    //        return;

    //    if (from == to) {
    //        int logical = logicalIndex(from);
    //        Q_ASSERT(logical != -1);
    //        updateSection(logical);
    //        return;
    //    }

    //    if (stretchLastSection() &&  to == d->lastVisibleVisualIndex())
    //        d->lastSectionSize = sectionSize(from);

    //    //int oldHeaderLength = length(); // ### for debugging; remove later
    //    d->initializeIndexMapping();

    //    QBitArray sectionHidden = d->sectionHidden;
    //    int *visualIndices = d->visualIndices.data();
    //    int *logicalIndices = d->logicalIndices.data();
    //    int logical = logicalIndices[from];
    //    int visual = from;

    //    int affected_count = qAbs(to - from) + 1;
    //    QVarLengthArray<int> sizes(affected_count);
    //    QVarLengthArray<ResizeMode> modes(affected_count);

    //    // move sections and indices
    //    if (to > from) {
    //        sizes[to - from] = d->headerSectionSize(from);
    //        modes[to - from] = d->headerSectionResizeMode(from);
    //        while (visual < to) {
    //            sizes[visual - from] = d->headerSectionSize(visual + 1);
    //            modes[visual - from] = d->headerSectionResizeMode(visual + 1);
    //            if (!sectionHidden.isEmpty())
    //                sectionHidden.setBit(visual, sectionHidden.testBit(visual + 1));
    //            visualIndices[logicalIndices[visual + 1]] = visual;
    //            logicalIndices[visual] = logicalIndices[visual + 1];
    //            ++visual;
    //        }
    //    } else {
    //        sizes[0] = d->headerSectionSize(from);
    //        modes[0] = d->headerSectionResizeMode(from);
    //        while (visual > to) {
    //            sizes[visual - to] = d->headerSectionSize(visual - 1);
    //            modes[visual - to] = d->headerSectionResizeMode(visual - 1);
    //            if (!sectionHidden.isEmpty())
    //                sectionHidden.setBit(visual, sectionHidden.testBit(visual - 1));
    //            visualIndices[logicalIndices[visual - 1]] = visual;
    //            logicalIndices[visual] = logicalIndices[visual - 1];
    //            --visual;
    //        }
    //    }
    //    if (!sectionHidden.isEmpty()) {
    //        sectionHidden.setBit(to, d->sectionHidden.testBit(from));
    //        d->sectionHidden = sectionHidden;
    //    }
    //    visualIndices[logical] = to;
    //    logicalIndices[to] = logical;

    //    //Q_ASSERT(oldHeaderLength == length());
    //    // move sizes
    //    // ### check for spans of section sizes here
    //    if (to > from) {
    //        for (visual = from; visual <= to; ++visual) {
    //            int size = sizes[visual - from];
    //            ResizeMode mode = modes[visual - from];
    //            d->createSectionSpan(visual, visual, size, mode);
    //        }
    //    } else {
    //        for (visual = to; visual <= from; ++visual) {
    //            int size = sizes[visual - to];
    //            ResizeMode mode = modes[visual - to];
    //            d->createSectionSpan(visual, visual, size, mode);
    //        }
    //    }
    //    //Q_ASSERT(d->headerLength() == length());
    //    //Q_ASSERT(oldHeaderLength == length());
    //    //Q_ASSERT(d->logicalIndices.count() == d->sectionCount);

    //    if (d->hasAutoResizeSections())
    //        d->doDelayedResizeSections();
    //    d->viewport->update();

    //    emit sectionMoved(logical, from, to);
}

/*!
    \since 4.2
    Swaps the section at visual index \a first with the section at visual
    index \a second.

    \sa moveSection()
*/
void QpHorHeaderView::swapSections(int first, int second)
{
    Q_D(QpHorHeaderView);
    if( debug )  qDebug()<<"QpHorHeaderView::swapSections";

    //    if (first == second)
    //        return;
    //    d->executePostedLayout();
    //    if (first < 0 || first >= d->sectionCount || second < 0 || second >= d->sectionCount)
    //        return;

    //    int firstSize = d->headerSectionSize(first);
    //    ResizeMode firstMode = d->headerSectionResizeMode(first);
    //    int firstLogical = d->logicalIndex(first);

    //    int secondSize = d->headerSectionSize(second);
    //    ResizeMode secondMode = d->headerSectionResizeMode(second);
    //    int secondLogical = d->logicalIndex(second);

    //    d->createSectionSpan(second, second, firstSize, firstMode);
    //    d->createSectionSpan(first, first, secondSize, secondMode);

    //    d->initializeIndexMapping();

    //    d->visualIndices[firstLogical] = second;
    //    d->logicalIndices[second] = firstLogical;

    //    d->visualIndices[secondLogical] = first;
    //    d->logicalIndices[first] = secondLogical;

    //    if (!d->sectionHidden.isEmpty()) {
    //        bool firstHidden = d->sectionHidden.testBit(first);
    //        bool secondHidden = d->sectionHidden.testBit(second);
    //        d->sectionHidden.setBit(first, secondHidden);
    //        d->sectionHidden.setBit(second, firstHidden);
    //    }

    //    d->viewport->update();
    //    emit sectionMoved(firstLogical, first, second);
    //    emit sectionMoved(secondLogical, second, first);
}


void QpHorHeaderView::resizeSection(int logical, int newSize)
{
    Q_D(QpHorHeaderView);

    if( debug ) qDebug()<<"QpHorHeaderView::resizeSection logical " << logical << "  newSize " << newSize;

    if (logical < 0 || logical >= count())
        return;

    if (isSectionHidden(logical))
    {
        d->hiddenSectionSize.insert(logical, newSize);
        return;
    }

    int visual = visualIndex(logical);

    if (visual == -1)
        return;

    int oldSize = d->headerSectionSize(visual);

    if (oldSize == newSize)
        return;

    d->executePostedLayout();

    d->invalidateCachedSizeHint();

    if (stretchLastSection() && visual == d->lastVisibleVisualIndex())
        d->lastSectionSize = newSize;

    if (newSize != oldSize)
    {
        //        d->createSectionSpan(visual, visual, size, d->headerSectionResizeMode(visual));
        d->setHeaderSectionSize(visual , newSize );
    }

    int w = d->viewport->width();

    int h = d->viewport->height();

    //int pos = sectionViewportPosition2(logical).x();

    int pos = left_common_border_x( logical );

    QRect rect = QRect(pos, 0, w - pos, h);

    if (d->hasAutoResizeSections())
    {
        d->doDelayedResizeSections();
        rect = d->viewport->rect();
    }

    d->viewport->update(rect.normalized());

    emit sectionResized(logical, oldSize, newSize);
}

void QpHorHeaderView::resizeSection_Y(int line, int newHeight)
{
    Q_D(QpHorHeaderView);

    if( debug ) qDebug()<<"QpHorHeaderView::resizeSection_Y line " << line << "  newHeight " << newHeight;

    if (line < 0 || line >= d->offsets_y.count() - 1)
        return;

    int oldHeight = d->offsets_y[ line +1 ] - d->offsets_y[ line ];

    if (oldHeight == newHeight)
        return;

    d->executePostedLayout();

    d->invalidateCachedSizeHint();

    //    if (isSectionHidden(logical))
    //    {
    //        d->hiddenSectionSize.insert(logical, newSize);
    //        return;
    //    }

    //    int visual = visualIndex(logical);

    //    if (visual == -1)
    //        return;

    int hh1 = row_height();

    QRect rect = geometry();
    int h1 = rect.height();

    QRect rectV = d->viewport->geometry();
    int hh1v = rectV.height();

    qDebug() << " cursor y:"<< QCursor::pos().y();

    //    if (stretchLastSection() && visual == d->lastVisibleVisualIndex())
    //        d->lastSectionSize = newHeight;

    //    if (newHeight != oldHeight)
    //    {
    //        d->setHeaderSectionSize(visual , newHeight );
    //    }

    d->recalculateLineHeigth_from( line , newHeight );

    //    int w = d->viewport->width();

    //    int h = d->viewport->height();

    int hh2 = row_height();

    int hh3 = rectV.height();

    d->viewport->update( );

    //    if( delta != 0 )
    //    {
    setMinimumHeight( row_height() - 1 );

    //emit geometriesChanged();
    emit sectionResized_Y( );
    //    }
    return;

    //    int pos = sectionViewportPosition2(logical).x();

    //    QRect rect = QRect(pos, 0, w - pos, h); // w - pos : remain

    //    if (d->hasAutoResizeSections())
    //    {
    //        d->doDelayedResizeSections();

    //        rect = d->viewport->rect();
    //    }

    //d->viewport->update( ); //rect.normalized() );

    //emit sectionResized(logical, oldHeight, newHeight);

}

/*!
    Resizes the sections according to the given \a mode, ignoring the current
    resize mode.

    \sa resizeMode(), sectionResized()
*/

void QpHorHeaderView::resizeSections(QpHorHeaderView::ResizeMode mode)
{
    Q_D(QpHorHeaderView);
    d->resizeSections(mode, true);
}


bool QpHorHeaderView::isSectionHidden(int logicalIndex) const
{
    Q_D(const QpHorHeaderView);

    d->executePostedLayout();

    if (logicalIndex >= d->sectionHidden.count() || logicalIndex < 0 || logicalIndex >= d->sectionCount)
        return false;

    return d->sectionHidden.testBit( logicalIndex );
}

/*!
    \since 4.1

    Returns the number of sections in the header that has been hidden.

    \sa setSectionHidden(), isSectionHidden()
*/
int QpHorHeaderView::hiddenSectionCount() const
{
    Q_D(const QpHorHeaderView);
    return d->hiddenSectionSize.count();
}

/*!
  If \a hide is true the section specified by \a logicalIndex is hidden;
  otherwise the section is shown.

  \sa isSectionHidden(), hiddenSectionCount()
*/

void QpHorHeaderView::setSectionHidden(int logicalIndex, bool hide)
{
    Q_D(QpHorHeaderView);

    if (logicalIndex < 0 || logicalIndex >= count())
        return;

    d->executePostedLayout();

    int visual = visualIndex(logicalIndex);

    Q_ASSERT(visual != -1);

    if (hide == d->isVisualIndexHidden(visual))
        return;

    if (hide)
    {
        int size = d->headerSectionSize(visual);
        if (!d->hasAutoResizeSections())
            resizeSection(logicalIndex, 0);

        d->hiddenSectionSize.insert(logicalIndex, size);

        if (d->sectionHidden.count() < count())
            d->sectionHidden.resize(count());

        d->sectionHidden.setBit(visual, true);

        if (d->hasAutoResizeSections())
            d->doDelayedResizeSections();
    }
    else
    {
        int size = d->hiddenSectionSize.value(logicalIndex, d->defaultSectionSize);
        d->hiddenSectionSize.remove(logicalIndex);

        if (d->hiddenSectionSize.isEmpty())
        {
            d->sectionHidden.clear();
        }
        else
        {
            Q_ASSERT(visual <= d->sectionHidden.count());
            d->sectionHidden.setBit(visual, false);
        }
        resizeSection(logicalIndex, size);
    }
}

/*!
    Returns the number of sections in the header.

    \sa  sectionCountChanged(), length()
*/
const int QpHorHeaderView::lines_count() const
{
    Q_D(const QpHorHeaderView);

    return d->offsets_y.count() - 1;
}

int QpHorHeaderView::left_common_border_x( int logicalIndex) const // for repaint only with column resized
{
    Q_D(const QpHorHeaderView);

    if( d->offsets_y.count() < 1 )
        return logicalIndex;

    if( ! d->map.contains( logicalIndex )  )
        return -1;

    int num = d->map[ logicalIndex ].right - 1; //  cell resized from right border forever

    int left_num = num;

    //int curr_left = num;

    int curr_line = d->map[ logicalIndex ].bottom - 1;

    QList<int> currLine =  d->visual_matrix[ curr_line ];

    while( left_num - 1 >=0 && currLine[ left_num - 1] == logicalIndex ) // current line
    {
        left_num--;
    }


    foreach ( QList<int> lst, d->visual_matrix) // lines
    {


        if( lst[ num ] != logicalIndex ) // other index cell
        {
            int other_Index = lst[ num ];

            int from = num ;

            while( from - 1 >=0 && lst[ from - 1] == other_Index )
            {
                from--;
            }
            left_num = qMin ( from , left_num );

        }
    }

    int x = d->offsets_x.value( left_num );

    return  x;
}

int QpHorHeaderView::count_of_section() const
{
    Q_D(const QpHorHeaderView);

    //Q_ASSERT(d->sectionCount == d->headerSectionCount());
    // ### this may affect the lazy layout
    d->executePostedLayout();

    return d->offsets_x.count() - 1 ;
}
int QpHorHeaderView::count() const
{
    Q_D(const QpHorHeaderView);
    //Q_ASSERT(d->sectionCount == d->headerSectionCount());
    // ### this may affect the lazy layout
    d->executePostedLayout();
    //return d->sectionCount;
    return d->map.count();
}

/*!
    Returns the visual index position of the section specified by the given
    \a logicalIndex, or -1 otherwise.

    Hidden sections still have valid visual indexes.

    \sa logicalIndex()
*/

int QpHorHeaderView::visualIndex(int logicalIndex) const
{
    Q_D(const QpHorHeaderView);

    return logicalIndex;

    //    if (logicalIndex < 0)
    //        return -1;

    //    d->executePostedLayout();

    //    if ( d->map.contains( logicalIndex ) )
    //    {
    //        int visual = logicalIndex;

    //        //Q_ASSERT(visual < d->sectionCount);

    //        //        if( debug )
    //        //        qDebug() << "QpHorHeaderView::visualIndex(logicalIndex:"<<logicalIndex<<")=" << visual;

    //        return visual;
    //    }

    //    return -1;
}


int QpHorHeaderView::logicalIndex(int visualIndex) const
{
    Q_D(const QpHorHeaderView);

    return visualIndex;

    //    if (visualIndex < 0 || visualIndex >= d->sectionCount)
    //        return -1;

    //    return d->logicalIndex(visualIndex);
}

int QpHorHeaderView::logicalIndex_atNum_x_line(int num_x, int line) const
{
    Q_D(const QpHorHeaderView);

    if ( line < 0 || line >= d->visual_matrix.count() )
        return -1;

    if ( num_x < 0 || num_x >= d->visual_matrix[ line ].count() )
        return -1;


    int idx = d->visual_matrix[ line ] [ num_x ];

    return idx;
}


// ### Qt 5: change to setSectionsMovable()
void QpHorHeaderView::setMovable(bool movable)
{
    Q_D(QpHorHeaderView);
    d->movableSections = movable;
}

/*!
    Returns true if the header can be moved by the user; otherwise returns
    false.

    \sa setMovable()
*/

// ### Qt 5: change to sectionsMovable()
bool QpHorHeaderView::isMovable() const
{
    Q_D(const QpHorHeaderView);
    return d->movableSections;
}

/*!
    If \a clickable is true, the header will respond to single clicks.

    \sa isClickable(), sectionClicked(), sectionPressed(),
    setSortIndicatorShown()
*/

// ### Qt 5: change to setSectionsClickable()
void QpHorHeaderView::setClickable(bool clickable)
{
    Q_D(QpHorHeaderView);
    d->clickableSections = clickable;
}

/*!
    Returns true if the header is clickable; otherwise returns false. A
    clickable header could be set up to allow the user to change the
    representation of the data in the view related to the header.

    \sa setClickable()
*/

// ### Qt 5: change to sectionsClickable()
bool QpHorHeaderView::isClickable() const
{
    Q_D(const QpHorHeaderView);
    return d->clickableSections;
}

void QpHorHeaderView::setHighlightSections(bool highlight)
{
    Q_D(QpHorHeaderView);
    d->highlightSelected = highlight;
}

bool QpHorHeaderView::highlightSections() const
{
    Q_D(const QpHorHeaderView);
    return d->highlightSelected;
}

/*!
    Sets the constraints on how the header can be resized to those described
    by the given \a mode.

    \sa resizeMode(), length(), sectionResized(), sectionAutoResize()
*/

void QpHorHeaderView::setResizeMode(ResizeMode mode)
{
    Q_D(QpHorHeaderView);
    initializeSections();
    d->stretchSections = (mode == Stretch ? count() : 0);
    d->contentsSections =  (mode == ResizeToContents ? count() : 0);
    d->setGlobalHeaderResizeMode(mode);
    if (d->hasAutoResizeSections())
        d->doDelayedResizeSections(); // section sizes may change as a result of the new mode
}

/*!
    \overload

    Sets the constraints on how the section specified by \a logicalIndex in
    the header can be resized to those described by the given \a mode. The logical
    index should exist at the time this function is called.

    \note This setting will be ignored for the last section if the stretchLastSection
    property is set to true. This is the default for the horizontal headers provided
    by QTreeView.

    \sa setStretchLastSection()
*/

// ### Qt 5: change to setSectionResizeMode()
void QpHorHeaderView::setResizeMode(int logicalIndex, ResizeMode mode)
{
    Q_D(QpHorHeaderView);
    int visual = visualIndex(logicalIndex);
    Q_ASSERT(visual != -1);

    ResizeMode old = d->headerSectionResizeMode(visual);
    d->setHeaderSectionResizeMode(visual, mode);

    if (mode == Stretch && old != Stretch)
        ++d->stretchSections;
    else if (mode == ResizeToContents && old != ResizeToContents)
        ++d->contentsSections;
    else if (mode != Stretch && old == Stretch)
        --d->stretchSections;
    else if (mode != ResizeToContents && old == ResizeToContents)
        --d->contentsSections;

    if (d->hasAutoResizeSections() && d->state == QpHorHeaderViewPrivate::NoState)
        d->doDelayedResizeSections(); // section sizes may change as a result of the new mode
}

/*!
    Returns the resize mode that applies to the section specified by the given
    \a logicalIndex.

    \sa setResizeMode()
*/

QpHorHeaderView::ResizeMode QpHorHeaderView::resizeMode(int logicalIndex) const
{
    Q_D(const QpHorHeaderView);

    int visual = visualIndex(logicalIndex);

    if (visual == -1)
        return Fixed; //the default value

    return d->headerSectionResizeMode(visual);
}

/*!
    \since 4.1

    Returns the number of sections that are set to resize mode stretch. In
    views, this can be used to see if the headerview needs to resize the
    sections when the view's geometry changes.

    \sa stretchLastSection, resizeMode()
*/

int QpHorHeaderView::stretchSectionCount() const
{
    Q_D(const QpHorHeaderView);
    return d->stretchSections;
}

/*!
  \property QpHorHeaderView::showSortIndicator
  \brief whether the sort indicator is shown

  By default, this property is false.

  \sa setClickable()
*/

void QpHorHeaderView::setSortIndicatorShown(bool show)
{
    Q_D(QpHorHeaderView);
    if (d->sortIndicatorShown == show)
        return;

    d->sortIndicatorShown = show;

    if (sortIndicatorSection() < 0 || sortIndicatorSection() > count())
        return;

    if (d->headerSectionResizeMode(sortIndicatorSection()) == ResizeToContents)
        resizeSections();

    d->viewport->update();
}

bool QpHorHeaderView::isSortIndicatorShown() const
{
    Q_D(const QpHorHeaderView);
    return d->sortIndicatorShown;
}

/*!
    Sets the sort indicator for the section specified by the given
    \a logicalIndex in the direction specified by \a order, and removes the
    sort indicator from any other section that was showing it.

    \a logicalIndex may be -1, in which case no sort indicator will be shown
    and the model will return to its natural, unsorted order. Note that not
    all models support this and may even crash in this case.

    \sa sortIndicatorSection() sortIndicatorOrder()
*/

void QpHorHeaderView::setSortIndicator(int logicalIndex, Qt::SortOrder order)
{
    Q_D(QpHorHeaderView);

    // This is so that people can set the position of the sort indicator before the fill the model
    int old = d->sortIndicatorSection;
    d->sortIndicatorSection = logicalIndex;
    d->sortIndicatorOrder = order;

    if (logicalIndex >= d->sectionCount) {
        emit sortIndicatorChanged(logicalIndex, order);
        return; // nothing to do
    }

    if (old != logicalIndex
            && ((logicalIndex >= 0 && resizeMode(logicalIndex) == ResizeToContents)
                || old >= d->sectionCount || (old >= 0 && resizeMode(old) == ResizeToContents))) {
        resizeSections();
        d->viewport->update();
    } else {
        if (old >= 0 && old != logicalIndex)
            updateSection(old);
        if (logicalIndex >= 0)
            updateSection(logicalIndex);
    }

    emit sortIndicatorChanged(logicalIndex, order);
}

/*!
    Returns the logical index of the section that has a sort indicator.
    By default this is section 0.

    \sa setSortIndicator() sortIndicatorOrder() setSortIndicatorShown()
*/

int QpHorHeaderView::sortIndicatorSection() const
{
    Q_D(const QpHorHeaderView);
    return d->sortIndicatorSection;
}

/*!
    Returns the order for the sort indicator. If no section has a sort
    indicator the return value of this function is undefined.

    \sa setSortIndicator() sortIndicatorSection()
*/

Qt::SortOrder QpHorHeaderView::sortIndicatorOrder() const
{
    Q_D(const QpHorHeaderView);
    return d->sortIndicatorOrder;
}

/*!
    \property QpHorHeaderView::stretchLastSection
    \brief whether the last visible section in the header takes up all the
    available space

    The default value is false.

    \note The horizontal headers provided by QTreeView are configured with this
    property set to true, ensuring that the view does not waste any of the
    space assigned to it for its header. If this value is set to true, this
    property will override the resize mode set on the last section in the
    header.

    \sa setResizeMode()
*/
bool QpHorHeaderView::stretchLastSection() const
{
    Q_D(const QpHorHeaderView);
    return d->stretchLastSection;
}

void QpHorHeaderView::setStretchLastSection(bool stretch)
{
    Q_D(QpHorHeaderView);
    d->stretchLastSection = stretch;
    if (d->state != QpHorHeaderViewPrivate::NoState)
        return;
    if (stretch)
        resizeSections();
    else if (count())
        resizeSection(count() - 1, d->defaultSectionSize);
}

/*!
    \since 4.2
    \property QpHorHeaderView::cascadingSectionResizes
    \brief whether interactive resizing will be cascaded to the following
    sections once the section being resized by the user has reached its
    minimum size

    This property only affects sections that have \l Interactive as their
    resize mode.

    The default value is false.

    \sa setResizeMode()
*/
bool QpHorHeaderView::cascadingSectionResizes() const
{
    Q_D(const QpHorHeaderView);
    return d->cascadingResizing;
}

void QpHorHeaderView::setCascadingSectionResizes(bool enable)
{
    Q_D(QpHorHeaderView);
    d->cascadingResizing = enable;
}

/*!
    \property QpHorHeaderView::defaultSectionSize
    \brief the default size of the header sections before resizing.

    This property only affects sections that have \l Interactive or \l Fixed
    as their resize mode.

    \sa setResizeMode() minimumSectionSize
*/
int QpHorHeaderView::defaultSectionSize() const
{
    Q_D(const QpHorHeaderView);
    return d->defaultSectionSize;
}

void QpHorHeaderView::setDefaultSectionSize(int size) // nowhere calls
{
    Q_D(QpHorHeaderView);
    d->setDefaultSectionSize(size);
}

/*!
    \since 4.2
    \property QpHorHeaderView::minimumSectionSize
    \brief the minimum size of the header sections.

    The minimum section size is the smallest section size allowed. If the
    minimum section size is set to -1, QpHorHeaderView will use the maximum of
    the \l{QApplication::globalStrut()}{global strut} or the
    \l{fontMetrics()}{font metrics} size.

    This property is honored by all \l{ResizeMode}{resize modes}.

    \sa setResizeMode() defaultSectionSize
*/
int QpHorHeaderView::minimumSectionSize() const
{
    Q_D(const QpHorHeaderView);

    if (d->minimumSectionSize == -1)
    {
        QSize strut = QApplication::globalStrut();

        int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);

        return qMax(strut.width(), (fontMetrics().maxWidth() + margin));

    }

    return d->minimumSectionSize;
}

void QpHorHeaderView::setMinimumSectionSize(int size)
{
    Q_D(QpHorHeaderView);
    d->minimumSectionSize = size;
}

/*!
    \since 4.1
    \property QpHorHeaderView::defaultAlignment
    \brief the default alignment of the text in each header section
*/

Qt::Alignment QpHorHeaderView::defaultAlignment() const
{
    Q_D(const QpHorHeaderView);
    return d->defaultAlignment;
}

void QpHorHeaderView::setDefaultAlignment(Qt::Alignment alignment)
{
    Q_D(QpHorHeaderView);
    if (d->defaultAlignment == alignment)
        return;

    d->defaultAlignment = alignment;
    d->viewport->update();
}

/*!
    \internal
*/
void QpHorHeaderView::doItemsLayout()
{
    initializeSections();
    QAbstractItemView::doItemsLayout();
}

/*!
    Returns true if sections in the header has been moved; otherwise returns
    false;

    \sa moveSection()
*/
bool QpHorHeaderView::sectionsMoved() const
{
    Q_D(const QpHorHeaderView);
    return !d->visualIndices.isEmpty();
}

/*!
    \since 4.1

    Returns true if sections in the header has been hidden; otherwise returns
    false;

    \sa setSectionHidden()
*/
bool QpHorHeaderView::sectionsHidden() const
{
    Q_D(const QpHorHeaderView);
    return !d->hiddenSectionSize.isEmpty();
}

#ifndef QT_NO_DATASTREAM
/*!
    \since 4.3

    Saves the current state of this header view.

    To restore the saved state, pass the return value to restoreState().

    \sa restoreState()
*/
QByteArray QpHorHeaderView::saveState() const
{
    Q_D(const QpHorHeaderView);
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << QpHorHeaderViewPrivate::VersionMarker;
    stream << 0; // current version is 0
    d->write(stream);
    return data;
}

/*!
    \since 4.3
    Restores the \a state of this header view.
    This function returns \c true if the state was restored; otherwise returns
    false.

    \sa saveState()
*/
bool QpHorHeaderView::restoreState(const QByteArray &state)
{
    Q_D(QpHorHeaderView);
    if (state.isEmpty())
        return false;
    QByteArray data = state;
    QDataStream stream(&data, QIODevice::ReadOnly);
    int marker;
    int ver;
    stream >> marker;
    stream >> ver;
    if (stream.status() != QDataStream::Ok
            || marker != QpHorHeaderViewPrivate::VersionMarker
            || ver != 0) // current version is 0
        return false;

    if (d->read(stream)) {
        emit sortIndicatorChanged(d->sortIndicatorSection, d->sortIndicatorOrder );
        d->viewport->update();
        return true;
    }
    return false;
}
#endif // QT_NO_DATASTREAM

/*!
  \reimp
*/
void QpHorHeaderView::reset()
{
    QAbstractItemView::reset();
    // it would be correct to call clear, but some apps rely
    // on the header keeping the sections, even after calling reset
    //d->clear();
    initializeSections();
}

/*!
    Updates the changed header sections with the given \a orientation, from
    \a logicalFirst to \a logicalLast inclusive.
*/
void QpHorHeaderView::headerDataChanged(Qt::Orientation orientation, int logicalFirst, int logicalLast)
{
    Q_D(QpHorHeaderView);
    if (Qt::Horizontal != orientation)
        return;

    if (logicalFirst < 0 || logicalLast < 0 || logicalFirst >= count() || logicalLast >= count())
        return;

    d->invalidateCachedSizeHint();

    int firstVisualIndex = INT_MAX, lastVisualIndex = -1;

    for (int section = logicalFirst; section <= logicalLast; ++section) {
        const int visual = visualIndex(section);
        firstVisualIndex = qMin(firstVisualIndex, visual);
        lastVisualIndex =  qMax(lastVisualIndex,  visual);
    }

    d->executePostedResize();

    const int first = d->headerSectionPosition2(firstVisualIndex).y();

    const int last = d->headerSectionPosition2(lastVisualIndex).y()
            + d->headerSectionSize(lastVisualIndex);


    d->viewport->update(first, 0, last - first, d->viewport->height());
}

/*!
    \internal
    \since 4.2

    Updates the section specified by the given \a logicalIndex.
*/

void QpHorHeaderView::updateSection(int logicalIndex)
{
    Q_D(QpHorHeaderView);

    d->viewport->update(QRect( sectionViewportPosition2(logicalIndex).x() ,
                               0, sectionSize(logicalIndex), d->viewport->height()));
}

/*!
    Resizes the sections according to their size hints. Normally, you do not
    have to call this function.
*/

void QpHorHeaderView::resizeSections()
{
    Q_D(QpHorHeaderView);

    if (d->hasAutoResizeSections())
        d->resizeSections(Interactive, false); // no global resize mode
}

/*!
    This slot is called when sections are inserted into the \a parent.
    \a logicalFirst and \a logicalLast indices signify where the new sections
    were inserted.

    If only one section is inserted, \a logicalFirst and \a logicalLast will
    be the same.
*/

void QpHorHeaderView::sectionsInserted(const QModelIndex &parent,
                                       int logicalFirst, int logicalLast)
{
    Q_D(QpHorHeaderView);

    if( debug )   qDebug()<<"QpHorHeaderView::sectionsInserted";

    //    if (parent != d->root)
    //        return; // we only handle changes in the top level

    //    int oldCount = d->sectionCount;

    //    d->invalidateCachedSizeHint();

    //    // add the new sections
    //    int insertAt = 0;

    //    for (int spanStart = 0; insertAt < d->sectionSpans.count() && spanStart < logicalFirst; ++insertAt)
    //        spanStart += d->sectionSpans.at(insertAt).count;

    //    int insertCount = logicalLast - logicalFirst + 1;

    //    d->sectionCount += insertCount;

    //    if (d->sectionSpans.isEmpty() || insertAt >= d->sectionSpans.count())
    //    {
    //        int insertLength = d->defaultSectionSize * insertCount;
    //        d->length += insertLength;
    //        QpHorHeaderViewPrivate::SectionSpan span(insertLength, insertCount, d->globalResizeMode);
    //        d->sectionSpans.append(span);
    //    } else if ((d->sectionSpans.at(insertAt).sectionSize() == d->defaultSectionSize)
    //               && d->sectionSpans.at(insertAt).resizeMode == d->globalResizeMode) {
    //        // add the new sections to an existing span
    //        int insertLength = d->sectionSpans.at(insertAt).sectionSize() * insertCount;
    //        d->length += insertLength;
    //        d->sectionSpans[insertAt].size += insertLength;
    //        d->sectionSpans[insertAt].count += insertCount;
    //    } else {
    //        // separate them out into their own span
    //        int insertLength = d->defaultSectionSize * insertCount;
    //        d->length += insertLength;
    //        QpHorHeaderViewPrivate::SectionSpan span(insertLength, insertCount, d->globalResizeMode);
    //        d->sectionSpans.insert(insertAt, span);
    //    }

    //    // update sorting column
    //    if (d->sortIndicatorSection >= logicalFirst)
    //        d->sortIndicatorSection += insertCount;

    //    // update resize mode section counts
    //    if (d->globalResizeMode == Stretch)
    //        d->stretchSections = d->sectionCount;
    //    else if (d->globalResizeMode == ResizeToContents)
    //        d->contentsSections = d->sectionCount;

    //    // clear selection cache
    //    d->sectionSelected.clear();

    //    // update mapping
    //    if (!d->visualIndices.isEmpty() && !d->logicalIndices.isEmpty())
    //    {
    //        Q_ASSERT(d->visualIndices.count() == d->logicalIndices.count());
    //        int mappingCount = d->visualIndices.count();

    //        for (int i = 0; i < mappingCount; ++i)
    //        {
    //            if (d->visualIndices.at(i) >= logicalFirst)
    //                d->visualIndices[i] += insertCount;
    //            if (d->logicalIndices.at(i) >= logicalFirst)
    //                d->logicalIndices[i] += insertCount;
    //        }

    //        for (int j = logicalFirst; j <= logicalLast; ++j)
    //        {
    //            d->visualIndices.insert(j, j);
    //            d->logicalIndices.insert(j, j);
    //        }
    //    }

    //    // insert sections into sectionsHidden
    //    if ( !d->sectionHidden.isEmpty())
    //    {
    //        QBitArray sectionHidden(d->sectionHidden);
    //        sectionHidden.resize(sectionHidden.count() + insertCount);
    //        sectionHidden.fill(false, logicalFirst, logicalLast + 1);

    //        for (int j = logicalLast + 1; j < sectionHidden.count(); ++j)
    //            //here we simply copy the old sectionHidden
    //            sectionHidden.setBit(j, d->sectionHidden.testBit(j - insertCount));

    //        d->sectionHidden = sectionHidden;
    //    }

    //    // insert sections into hiddenSectionSize

    //    QHash<int, int> newHiddenSectionSize; // from logical index to section size
    //    for (int i = 0; i < logicalFirst; ++i)
    //        if (isSectionHidden(i))
    //            newHiddenSectionSize[i] = d->hiddenSectionSize[i];

    //    for (int j = logicalLast + 1; j < d->sectionCount; ++j)
    //        if (isSectionHidden(j))
    //            newHiddenSectionSize[j] = d->hiddenSectionSize[j - insertCount];

    //    d->hiddenSectionSize = newHiddenSectionSize;

    //    d->doDelayedResizeSections();

    //    emit sectionCountChanged(oldCount, count());

    //    // if the new sections were not updated by resizing, we need to update now
    //    if (!d->hasAutoResizeSections())
    //        d->viewport->update();
}

/*!
    This slot is called when sections are removed from the \a parent.
    \a logicalFirst and \a logicalLast signify where the sections were removed.

    If only one section is removed, \a logicalFirst and \a logicalLast will
    be the same.
*/

void QpHorHeaderView::sectionsAboutToBeRemoved(const QModelIndex &parent,
                                               int logicalFirst, int logicalLast)
{
    Q_UNUSED(parent);
    Q_UNUSED(logicalFirst);
    Q_UNUSED(logicalLast);
}

void QpHorHeaderViewPrivate::setLines(int Lines)
{
    lines = Lines;
}

int QpHorHeaderViewPrivate::row_height() const
{
    int count = offsets_y.count();

    if( count == 0)
        return -1;

    return offsets_y.at( count - 1 );
}

qp::CELL_NODES QpHorHeaderViewPrivate::get_nodes( int logicalIndex )
{
    int visual = visualIndex( logicalIndex );

    if( visual == -1)
        return qp::CELL_NODES();

    //    if( ! map.contains( visual ) )
    //        return CELL_NODES();

    //    return map.value(  visual );


    qp::CELL_NODES cells;

    for( int num = 0 ; num < offsets_x.count() -1 ; num++ )
    {
        bool found_hor = false;

        for( int line = 0 ; line < visual_matrix.count(); line++ )
        {
            if( visual_matrix[ line ] [ num ] == visual )
            {
                if ( cells.top == qp::CELL_NODES::NODE_UNDEFINED )
                {
                    cells.top = line;
                    cells.bottom = line;
                }
                else
                    cells.bottom = line;

                if ( cells.left == qp::CELL_NODES::NODE_UNDEFINED )
                {
                    cells.left = num;
                    cells.right = num;
                }
                else
                    cells.right = num;
            }

        }
    }

    if( debug )  qDebug() << "QpHorHeaderViewPrivate::get_nodes (logicalIndex:" << logicalIndex <<") " << cells.left << cells.right << cells.top << cells.bottom;

    return cells;
}

void QpHorHeaderViewPrivate::recalculateLineHeigth_from( int from_line,  int height)
{
    if( from_line < 0 || from_line >= offsets_y.count() - 1)
        return;

    int before_h = offsets_y[ from_line + 1] - offsets_y[ from_line ];

    int delta = height - before_h ;

    offsets_y[ from_line + 1  ] =  offsets_y[ from_line + 1  ] + delta;

    for( int line = from_line + 2 ; line < offsets_y.count(); line++)
    {
        int y1 = offsets_y[ line] + delta;

        offsets_y[ line  ] =  y1;

        qDebug() << " height : " << height << " delta " << delta ;
    }


    if( debug_resize ) qDebug() << "recalculateLineHeigth_from line "<<from_line<<" delta " << delta<< " cursor y:"<< QCursor::pos().y();

    int ii=0;
    foreach( int yy, offsets_y)
        if( debug_resize ) qDebug() << ii++ << " yy: " << yy;


}


void QpHorHeaderViewPrivate::updateHiddenSections(int logicalFirst, int logicalLast)
{
    Q_Q(QpHorHeaderView);
    const int changeCount = logicalLast - logicalFirst + 1;

    // remove sections from hiddenSectionSize
    QHash<int, int> newHiddenSectionSize; // from logical index to section size
    for (int i = 0; i < logicalFirst; ++i)
        if (q->isSectionHidden(i))
            newHiddenSectionSize[i] = hiddenSectionSize[i];
    for (int j = logicalLast + 1; j < sectionCount; ++j)
        if (q->isSectionHidden(j))
            newHiddenSectionSize[j - changeCount] = hiddenSectionSize[j];
    hiddenSectionSize = newHiddenSectionSize;

    // remove sections from sectionsHidden
    if (!sectionHidden.isEmpty()) {
        const int newsize = qMin(sectionCount - changeCount, sectionHidden.size());
        QBitArray newSectionHidden(newsize);
        for (int j = 0, k = 0; j < sectionHidden.size(); ++j) {
            const int logical = logicalIndex(j);
            if (logical < logicalFirst || logical > logicalLast) {
                newSectionHidden[k++] = sectionHidden[j];
            }
        }
        sectionHidden = newSectionHidden;
    }
}

void QpHorHeaderViewPrivate::_q_sectionsRemoved(const QModelIndex &parent,
                                                int logicalFirst, int logicalLast)
{
    Q_Q(QpHorHeaderView);
    //    if (parent != root)
    //        return; // we only handle changes in the top level
    //    if (qMin(logicalFirst, logicalLast) < 0
    //            || qMax(logicalLast, logicalFirst) >= sectionCount)
    //        return;
    //    int oldCount = q->count();
    //    int changeCount = logicalLast - logicalFirst + 1;

    //    updateHiddenSections(logicalFirst, logicalLast);

    //    if (visualIndices.isEmpty() && logicalIndices.isEmpty())
    //    {
    //        //Q_ASSERT(headerSectionCount() == sectionCount);
    //        removeSectionsFromSpans(logicalFirst, logicalLast);
    //    }
    //    else
    //    {
    //        for (int l = logicalLast; l >= logicalFirst; --l)
    //        {
    //            int visual = visualIndices.at(l);
    //            for (int v = 0; v < sectionCount; ++v) {
    //                if (v >= logicalIndices.count())
    //                    continue; // the section doesn't exist
    //                if (v > visual) {
    //                    int logical = logicalIndices.at(v);
    //                    --(visualIndices[logical]);
    //                }
    //                if (logicalIndex(v) > l) // no need to move the positions before l
    //                    --(logicalIndices[v]);
    //            }
    //            logicalIndices.remove(visual);
    //            visualIndices.remove(l);
    //            //Q_ASSERT(headerSectionCount() == sectionCount);
    //            removeSectionsFromSpans(visual, visual);
    //        }
    //        // ### handle sectionSelection, sectionHidden
    //    }
    //    sectionCount -= changeCount;

    //    // update sorting column
    //    if (sortIndicatorSection >= logicalFirst) {
    //        if (sortIndicatorSection <= logicalLast)
    //            sortIndicatorSection = -1;
    //        else
    //            sortIndicatorSection -= changeCount;
    //    }

    //    // if we only have the last section (the "end" position) left, the header is empty
    //    if (sectionCount <= 0)
    //        clear();
    //    invalidateCachedSizeHint();
    //    emit q->sectionCountChanged(oldCount, q->count());
    //    viewport->update();
}

void QpHorHeaderViewPrivate::_q_layoutAboutToBeChanged()
{
    //if there is no row/column we can't have mapping for columns
    //because no QModelIndex in the model would be valid
    // ### this is far from being bullet-proof and we would need a real system to
    // ### map columns or rows persistently
    if ( model->rowCount(root) == 0    || model->columnCount(root) == 0)
        return;

    for (int i = 0; i < sectionHidden.count(); ++i)
        if (sectionHidden.testBit(i)) // ### note that we are using column or row 0
            persistentHiddenSections.append( model->index(0, logicalIndex(i), root));
}

void QpHorHeaderViewPrivate::_q_layoutChanged()
{
    Q_Q(QpHorHeaderView);
    viewport->update();
    if (persistentHiddenSections.isEmpty() || modelIsEmpty()) {
        if (modelSectionCount() != sectionCount)
            q->initializeSections();
        persistentHiddenSections.clear();
        return;
    }

    QBitArray oldSectionHidden = sectionHidden;
    bool sectionCountChanged = false;

    for (int i = 0; i < persistentHiddenSections.count(); ++i) {
        QModelIndex index = persistentHiddenSections.at(i);

        if (index.isValid())
        {
            const int logical =  index.column();

            q->setSectionHidden(logical, true);
            oldSectionHidden.setBit(logical, false);
        }
        else if
                (!sectionCountChanged && (modelSectionCount() != sectionCount))
        {
            sectionCountChanged = true;
            break;
        }
    }
    persistentHiddenSections.clear();

    for (int i = 0; i < oldSectionHidden.count(); ++i) {
        if (oldSectionHidden.testBit(i))
            q->setSectionHidden(i, false);
    }

    // the number of sections changed; we need to reread the state of the model
    if (sectionCountChanged)
        q->initializeSections();
}

/*!
  \internal
*/

void QpHorHeaderView::initializeSections()
{
    Q_D(QpHorHeaderView);

    const int oldCount = d->sectionCount;
    const int newCount = d->modelSectionCount();

    if (newCount <= 0)
    {
        d->clear();
        emit sectionCountChanged(oldCount, 0);
    }
    else if (newCount != oldCount)
    {
        const int min = qBound(0, oldCount, newCount - 1);
        initializeSections(min, newCount - 1);
        if (stretchLastSection()) // we've already gotten the size hint
            d->lastSectionSize = sectionSize(logicalIndex(d->sectionCount - 1));

        //make sure we update the hidden sections
        if (newCount < oldCount)
            d->updateHiddenSections(0, newCount-1);
    }
}

/*!
    \internal
*/

void QpHorHeaderView::initializeSections(int start, int end)
{
    Q_D(QpHorHeaderView);

    if( debug )  qDebug() << "QpHorHeaderView::initializeSections start:" << start << " end:"<<end;

    Q_ASSERT(start >= 0);
    Q_ASSERT(end >= 0);

    d->invalidateCachedSizeHint();

    if (end + 1 < d->sectionCount)
    {
        int newCount = end + 1;

        //d->removeSectionsFromSpans(newCount, d->sectionCount);

        if (!d->hiddenSectionSize.isEmpty())
        {
            if (d->sectionCount - newCount > d->hiddenSectionSize.count())
            {
                for (int i = end + 1; i < d->sectionCount; ++i)
                    d->hiddenSectionSize.remove(i);
            }
            else
            {
                QHash<int, int>::iterator it = d->hiddenSectionSize.begin();

                while (it != d->hiddenSectionSize.end())
                {
                    if (it.key() > end)
                        it = d->hiddenSectionSize.erase(it);
                    else
                        ++it;
                }
            }
        }
    }

    int oldCount = d->sectionCount;
    d->sectionCount = end + 1;

    if (!d->logicalIndices.isEmpty())
    {
        if (oldCount <= d->sectionCount)
        {
            d->logicalIndices.resize(d->sectionCount);
            d->visualIndices.resize(d->sectionCount);
            for (int i = oldCount; i < d->sectionCount; ++i)
            {
                d->logicalIndices[i] = i;
                d->visualIndices[i] = i;
            }
        }
        else
        {
            int j = 0;
            for (int i = 0; i < oldCount; ++i) {
                int v = d->logicalIndices.at(i);
                if (v < d->sectionCount) {
                    d->logicalIndices[j] = v;
                    d->visualIndices[v] = j;
                    j++;
                }
            }
            d->logicalIndices.resize(d->sectionCount);
            d->visualIndices.resize(d->sectionCount);
        }
    }

    if (d->globalResizeMode == Stretch)
        d->stretchSections = d->sectionCount;
    else if (d->globalResizeMode == ResizeToContents)
        d->contentsSections = d->sectionCount;
    if (!d->sectionHidden.isEmpty())
        d->sectionHidden.resize(d->sectionCount);

    if (d->sectionCount > oldCount)
    {
        //        d->createSectionSpan( start,
        //                              end,
        //                              (end - start + 1) * d->defaultSectionSize,
        //                              d->globalResizeMode);
    }

    //Q_ASSERT(d->headerLength() == d->length);

    if (d->sectionCount != oldCount)
        emit sectionCountChanged(oldCount,  d->sectionCount);
    d->viewport->update();
}

/*!
  \reimp
*/

void QpHorHeaderView::currentChanged(const QModelIndex &current, const QModelIndex &old)
{
    Q_D(QpHorHeaderView);

    if ( current.column() != old.column())
    {
        if (old.isValid() && old.parent() == d->root)

            d->viewport->update(QRect(sectionViewportPosition2(old.column()).x(), 0,
                                      sectionSize(old.column()), d->viewport->height()));

        if (current.isValid() && current.parent() == d->root)

            d->viewport->update(QRect(sectionViewportPosition2(current.column()).x(), 0,
                                      sectionSize(current.column()), d->viewport->height()));

    }

}


/*!
  \reimp
*/

bool QpHorHeaderView::event(QEvent *e)
{
    Q_D(QpHorHeaderView);


    switch (e->type())
    {
    case QEvent::HoverEnter:
    {
        if ( debug ) qDebug() << "QpHorHeaderView::event( HoverEnter )";

        QHoverEvent *he = static_cast<QHoverEvent*>(e);

        d->hover = logicalIndexAt( he->pos());

        if (d->hover != -1)
            updateSection(d->hover);
        break;
    }
    case QEvent::Leave:
    case QEvent::HoverLeave:
    {
        if ( debug ) qDebug() << "QpHorHeaderView::event( Leave |  HoverLeave)";

        if (d->hover != -1)
            updateSection(d->hover);
        d->hover = -1;
        break;
    }
    case QEvent::HoverMove:
    {
        if ( debug ) qDebug() << "QpHorHeaderView::event( HoverMove )";

        QHoverEvent *he = static_cast<QHoverEvent*>(e);

        int oldHover = d->hover;

        d->hover = logicalIndexAt( he->pos() );

        if (d->hover != oldHover)
        {
            if (oldHover != -1)
                updateSection(oldHover);
            if (d->hover != -1)
                updateSection(d->hover);
        }
        break;
    }
    case QEvent::Timer:
    {
        if ( debug ) qDebug() << "QpHorHeaderView::event( Timer )";

        QTimerEvent *te = static_cast<QTimerEvent*>(e);

        if (te->timerId() == d->delayedResize.timerId())
        {
            d->delayedResize.stop();

            resizeSections();
        }
        break;
    }
    default:
        break;
    }
    return QAbstractItemView::event(e);
}


void QpHorHeaderView::paintEvent(QPaintEvent *e)
{
    Q_D(QpHorHeaderView);

    if (count() == 0)
        return;

    QPainter painter( d->viewport );

    const QPoint offset = d->scrollDelayOffset;

    if( debug_scroll ) qDebug() << "QpHorHeaderView::paintEvent d->scrollDelayOffset:"<<d->scrollDelayOffset;

    QRect translatedEventRect = e->rect();

    if( debug_paint ) qDebug() << "QpHorHeaderView::paintEvent e->rect() : " << translatedEventRect << " d->scrollDelayOffset " << d->scrollDelayOffset;

    translatedEventRect.translate( offset );

    int start_x_num = -1;

    int end_x_num = -1;

    start_x_num = visual_xNum_At( translatedEventRect.left() );

    end_x_num = visual_xNum_At( translatedEventRect.right()  );

    start_x_num = (start_x_num == -1 ? 0 : start_x_num);

    end_x_num = (end_x_num == -1 ? d->offsets_x.count() - 2 : end_x_num);

    int tmp = start_x_num;

    start_x_num = qMin(start_x_num, end_x_num);

    end_x_num = qMax(tmp, end_x_num);

    d->prepareSectionSelected(); // clear and resize the bit array

    QRect currentSectionRect;

    int logical;

    const int width = d->viewport->width();

    const int height = d->viewport->height() ;


    for (int xNum = start_x_num; xNum <= end_x_num; ++xNum)
    {

        for( int line = 0; line < lines_count(); ++line)
        {
            //            if (d->isVisualIndexHidden(xNum))
            //                continue;
            painter.save();

            logical = logicalIndex_atNum_x_line( xNum , line );

            if( logical == -1)
            {
                painter.restore();
                qDebug() << "56839580984609865";
                continue;
            }

            //currentSectionRect.setRect( sectionViewportPosition2( logical ).x(), 0, sectionSize(logical), height);

            currentSectionRect = sectionViewportPosition2( logical );

            //currentSectionRect = d->headerSectionPosition2( i );

            currentSectionRect.translate( offset );

            QVariant variant = d->model->headerData(logical, Qt::Horizontal, Qt::FontRole);


            if (variant.isValid() && variant.canConvert<QFont>())
            {
                qDebug() << "fadfjlakdsjfkdsjklfd";
                QFont sectionFont = qvariant_cast<QFont>(variant);
                painter.setFont(sectionFont);
            }


            int ww = currentSectionRect.width();
            currentSectionRect.setWidth(  ww - d->grid + QpTableView::correct_width_minus_1 + 1 );

            int hh = currentSectionRect.height();
            currentSectionRect.setHeight( hh - d->grid + QpTableView::correct_height_minus_1 );


            //if ( debug ) qDebug() << " QpHorHeaderView::paintSection() rect " << currentSectionRect ;

            paintSection( &painter, currentSectionRect, logical );


            int x1 = currentSectionRect.left() ;
            int x2 = currentSectionRect.right() + d->grid * 2;
            int y = currentSectionRect.bottom() + 1;

            //if ( debug ) qDebug() << "QpHorHeaderView::paintEvent() drawLine x1 " << x1 << " x2 " << x2 << " y " << y;

            //        painter.drawLine( x1,
            //                          y,
            //                          x2,
            //                          y );

            painter.restore(); // !!!

        }


    }



    QStyleOption opt;
    opt.init(this);

    // Paint the area beyond where there are indexes

    //    if (d->reverse())
    //    {
    //        opt.state |= QStyle::State_Horizontal;

    //        if (currentSectionRect.left() > translatedEventRect.left())
    //        {
    //            opt.rect = QRect(translatedEventRect.left(), 0,
    //                             currentSectionRect.left() - translatedEventRect.left(), height);
    //            style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
    //        }
    //    }
    //    else
    //        if (currentSectionRect.right() < translatedEventRect.right())
    //        {
    //            // paint to the right
    //            opt.state |= QStyle::State_Horizontal;

    //            int x1 = currentSectionRect.right() + 1 ;
    //            int x2 = translatedEventRect.right() - currentSectionRect.right() + d->grid;
    //            int y2 = height;

    //            opt.rect = QRect( x1, 0,
    //                              x2 , y2);

    //            style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
    //        }
    //        else if (currentSectionRect.bottom() < translatedEventRect.bottom())
    //        {
    //            // paint the bottom section
    //            opt.state &= ~QStyle::State_Horizontal;

    //            opt.rect = QRect(0,
    //                             currentSectionRect.bottom() + 1,
    //                             width,
    //                             height - currentSectionRect.bottom() - 1);

    //            style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
    //        }

#if 0
    // ### visualize section spans
    for (int a = 0, i = 0; i < d->sectionSpans.count(); ++i) {
        QColor color((i & 4 ? 255 : 0), (i & 2 ? 255 : 0), (i & 1 ? 255 : 0));
        if (Qt::Horizontal == Qt::Horizontal)
            painter.fillRect(a - d->offset, 0, d->sectionSpans.at(i).size, 4, color);
        else
            painter.fillRect(0, a - d->offset, 4, d->sectionSpans.at(i).size, color);
        a += d->sectionSpans.at(i).size;
    }

#endif

}

/*!
  \reimp
*/

void QpHorHeaderView::mousePressEvent(QMouseEvent *e)
{
    Q_D(QpHorHeaderView);

    if (d->state != QpHorHeaderViewPrivate::NoState || e->button() != Qt::LeftButton)
        return;


    int xx = e->x();
    int yy = e->y();

    Qt::Orientation orientation = Qt::Horizontal;

    int handle = d->sectionHandleAt( e->pos() , orientation );

    if ( debug ) qDebug() << "QpHorHeaderView::mousePressEvent xx " << xx <<"  handle " << handle;

    d->originalSize_X = -1; // clear the stored original size
    d->originalSize_Y = -1; // clear the stored original size


    if (handle == -1)
    {
        d->pressed = logicalIndexAt( e->pos()  );

        if ( debug )
            qDebug() << "       d->pressed " << d->pressed;

        if (d->clickableSections)
            emit sectionPressed(d->pressed);

        if (d->movableSections)
        {
            d->section = d->target = d->pressed;

            if (d->section == -1)
                return;

            d->state = QpHorHeaderViewPrivate::MoveSection;
            d->setupSectionIndicator(d->section, xx);
        }
        else if (d->clickableSections && d->pressed != -1)
        {
            updateSection(d->pressed);
            d->state = QpHorHeaderViewPrivate::SelectSections;
        }
    }
    else if (resizeMode(handle) == Interactive)
    {
        if( orientation == Qt::Horizontal)
        {
            d->originalSize_X = sectionSize(handle);

            d->state = QpHorHeaderViewPrivate::ResizeSection_X;

            d->firstPos_x = xx;
            d->lastPos_x = xx;

            d->section = handle;

            if ( debug ) qDebug() << "       d->firstPos " << d->firstPos_x << " d->lastPos " << d->lastPos_x;

        }
        else if( orientation == Qt::Vertical)
        {
            //QRect rect = sectionPosition2( handle );

            qp::CELL_NODES cell = d->get_nodes( handle );

            if( ! cell.isEmpty() )
            {
                d->pressed_line = cell.bottom;
            }

            d->originalSize_Y = d->get_line_height( d->pressed_line );

            d->state = QpHorHeaderViewPrivate::ResizeSection_Y;

            d->firstPos_y = yy;
            d->lastPos_y = yy;

        }

        d->pressed_orientation = orientation;

    }

    d->clearCascadingSections();
}

bool QpHorHeaderViewPrivate::set_line_height( int line , int height)
{

    if( offsets_y.count() == 0 || line >= offsets_y.count())
        return false;

    recalculateLineHeigth_from( line , height );

    return true;

}

int QpHorHeaderViewPrivate::get_line_height( int line )
{
    if (line < 0 || line >= offsets_y.count() - 1)
        return -1;

    return offsets_y[ line + 1] - offsets_y[ line ];
}

int QpHorHeaderView::minimumLineHeight()
{
    return 10;
}

void QpHorHeaderView::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(QpHorHeaderView);

    int xx =  e->x() ;
    int yy =  e->y() ;

    if (xx < 0)
        return;

    if (e->buttons() == Qt::NoButton)
    {
#if !defined(Q_WS_MAC)
        // Under Cocoa, when the mouse button is released, may include an extra
        // simulated mouse moved event. The state of the buttons when this event
        // is generated is already "no button" and the code below gets executed
        // just before the mouseReleaseEvent and resets the state. This prevents
        // column dragging from working. So this code is disabled under Cocoa.
        d->state = QpHorHeaderViewPrivate::NoState;
        d->pressed = -1;
#endif
    }

    switch (d->state)
    {
    case QpHorHeaderViewPrivate::ResizeSection_X:
    {
        Q_ASSERT(d->originalSize_X != -1);

        if (d->cascadingResizing)
        {
            int delta =  xx - d->lastPos_x;

            int visual = visualIndex( d->section );

            d->cascadingResize(visual, d->headerSectionSize(visual) + delta);
        }
        else
        {

            int delta =  xx - d->firstPos_x;

            resizeSection( d->section, qMax(d->originalSize_X + delta, minimumSectionSize()));

            d->lastPos_x = xx;
        }


        return;
    }
    case QpHorHeaderViewPrivate::ResizeSection_Y:
    {
        Q_ASSERT(d->originalSize_Y != -1);

        if (d->cascadingResizing)
        {
            int delta =  yy - d->lastPos_y;

            int visual = visualIndex( d->section );

            d->cascadingResize(visual, d->headerSectionSize(visual) + delta);
        }
        else
        {
            if( d->pressed_line == -1)
                return;

            int delta =  yy - d->firstPos_y;

            //int oldHeight = d->line;

            resizeSection_Y( d->pressed_line , qMax( d->originalSize_Y + delta , minimumLineHeight()));


            d->lastPos_y = yy;

        }

        return;
    }
    case QpHorHeaderViewPrivate::MoveSection:
    {
        if (qAbs(xx - d->firstPos_x) >= QApplication::startDragDistance()
                || !d->sectionIndicator->isHidden())
        {
            int visual = visualIndexAt(xx , 1 );//??
            if (visual == -1)
                return;

            int posThreshold = d->headerSectionPosition2(visual).y() + d->headerSectionSize(visual) / 2;

            int moving = visualIndex(d->section);

            if (visual < moving)
            {
                if (xx < posThreshold)
                    d->target = d->logicalIndex(visual);
                else
                    d->target = d->logicalIndex(visual + 1);
            }
            else if (visual > moving)
            {
                if (xx > posThreshold)
                    d->target = d->logicalIndex(visual);
                else
                    d->target = d->logicalIndex(visual - 1);
            }
            else
            {
                d->target = d->section;
            }
            d->updateSectionIndicator(d->section, xx);
        }
        return;
    }
    case QpHorHeaderViewPrivate::SelectSections:
    {
        int logical = logicalIndexAt( e->pos() );

        if (logical == d->pressed)
            return; // nothing to do
        else if (d->pressed != -1)
            updateSection(d->pressed);

        d->pressed = logical;

        if (d->clickableSections && logical != -1)
        {
            emit sectionEntered(d->pressed);
            updateSection(d->pressed);
        }
        return;
    }
    case QpHorHeaderViewPrivate::NoState:
    {
#ifndef QT_NO_CURSOR

        Qt::Orientation orientation = Qt::Horizontal;

        int handle = d->sectionHandleAt( e->pos() , orientation ); // visual number

        bool hasCursor = testAttribute(Qt::WA_SetCursor);

        if ( handle != -1 && ( resizeMode( handle ) == Interactive))
        {
            if ( ! hasCursor )
            {
                if( orientation == Qt::Horizontal )
                {
                    setCursor( Qt::SplitHCursor );
                    if ( debug ) qDebug()<< "   mouseMoveEvent Qt::SplitHCursor ";
                }
                else
                {
                    setCursor( Qt::SplitVCursor );
                    if ( debug ) qDebug()<< "   mouseMoveEvent Qt::SplitVCursor ";
                }

            }
        }
        else if (hasCursor)
        {
            unsetCursor();
            if ( debug ) qDebug()<< "   mouseMoveEvent unsetCursor ";
        }
#endif

        return;
    }
    default:
        break;
    }
}


void QpHorHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(QpHorHeaderView);

    int xx = e->x();
    int yy =e->y();

    if ( debug ) qDebug() << "HeaderView2::mouseReleaseEven xx " << xx << " yy " << yy ;

    switch (d->state)
    {
    case QpHorHeaderViewPrivate::MoveSection:

        if (!d->sectionIndicator->isHidden())
        { // moving

            int from = visualIndex(d->section);

            Q_ASSERT(from != -1);

            int to = visualIndex(d->target);

            Q_ASSERT(to != -1);

            moveSection(from, to);
            d->section = d->target = -1;
            d->updateSectionIndicator(d->section, xx);

            break;
        } // not moving

    case QpHorHeaderViewPrivate::SelectSections:

        if (!d->clickableSections)
        {
            int section = logicalIndexAt( e->pos() );

            updateSection(section);
        }

        // fall through
    case QpHorHeaderViewPrivate::NoState:

        if (d->clickableSections)
        {
            int section = logicalIndexAt( e->pos() );

            if (section != -1 && section == d->pressed)
            {
                d->flipSortIndicator(section);

                emit sectionClicked(section);
            }

            if (d->pressed != -1)
                updateSection(d->pressed);
        }
        break;

    case QpHorHeaderViewPrivate::ResizeSection_X:

        d->originalSize_X = -1;
        d->clearCascadingSections();

        break;
    case QpHorHeaderViewPrivate::ResizeSection_Y:

        d->originalSize_Y = -1;
        d->pressed_line = -1;
        d->clearCascadingSections();

        break;
    default:
        break;
    }
    d->state = QpHorHeaderViewPrivate::NoState;
    d->pressed = -1;
}

/*!
  \reimp
*/

void QpHorHeaderView::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(QpHorHeaderView);

    int xx = e->x() ;

    Qt::Orientation orientation = Qt::Horizontal;

    int handle = d->sectionHandleAt( e->pos() , orientation);

    if (handle > -1 && resizeMode(handle) == Interactive)
    {
        emit sectionHandleDoubleClicked(handle);

#ifndef QT_NO_CURSOR

        Qt::CursorShape splitCursor = Qt::SplitHCursor ;

        if (cursor().shape() == splitCursor)
        {
            // signal handlers may have changed the section size

            Qt::Orientation orientation;

            handle = d->sectionHandleAt(e->pos() , orientation);

            if (!(handle > -1 && resizeMode(handle) == Interactive))
                setCursor(Qt::ArrowCursor);
        }
#endif
    }
    else
    {
        emit sectionDoubleClicked(logicalIndexAt(e->pos()));
    }
}

/*!
  \reimp
*/

bool QpHorHeaderView::viewportEvent(QEvent *e)
{
    Q_D(QpHorHeaderView);

    switch (e->type())
    {
#ifndef QT_NO_TOOLTIP
    case QEvent::ToolTip:
    {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        int logical = logicalIndexAt(he->pos());

        if (logical != -1)
        {
            QVariant variant = d->model->headerData(logical, Qt::Horizontal, Qt::ToolTipRole);

            if (variant.isValid())
            {
                QToolTip::showText(he->globalPos(), variant.toString(), this);
                return true;
            }
        }
        break; }
#endif
#ifndef QT_NO_WHATSTHIS
    case QEvent::QueryWhatsThis:
    {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        int logical = logicalIndexAt(he->pos());

        if (logical != -1
                && d->model->headerData(logical, Qt::Horizontal, Qt::WhatsThisRole).isValid())
            return true;
        break;
    }
    case QEvent::WhatsThis:
    {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        int logical = logicalIndexAt(he->pos());

        if (logical != -1)
        {
            QVariant whatsthis = d->model->headerData(logical, Qt::Horizontal,
                                                      Qt::WhatsThisRole);
            if (whatsthis.isValid())
            {
                QWhatsThis::showText(he->globalPos(), whatsthis.toString(), this);
                return true;
            }
        }
        break;
    }
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_STATUSTIP
    case QEvent::StatusTip:
    {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        int logical = logicalIndexAt(he->pos());

        if (logical != -1)
        {
            QString statustip = d->model->headerData(logical, Qt::Horizontal,
                                                     Qt::StatusTipRole).toString();
            if (!statustip.isEmpty())
                setStatusTip(statustip);
        }

        return true;
    }
#endif // QT_NO_STATUSTIP
    case QEvent::Hide:
    case QEvent::Show:
    case QEvent::FontChange:
    case QEvent::StyleChange:
        d->invalidateCachedSizeHint();
        resizeSections();
        emit geometriesChanged();
        break;
    case QEvent::ContextMenu:
    {
        d->state = QpHorHeaderViewPrivate::NoState;
        d->pressed = d->section = d->target = -1;
        d->updateSectionIndicator(d->section, -1);
        break;
    }
    case QEvent::Wheel:
    {
        QAbstractScrollArea *asa = qobject_cast<QAbstractScrollArea *>(parentWidget());
        if (asa)
            return QApplication::sendEvent(asa->viewport(), e);
        break;
    }
    default:
        break;
    }
    return QAbstractItemView::viewportEvent(e);
}

void QpHorHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // -----------------------------------------------------
    // paintSection is called from paintEvent
    // -----------------------------------------------------
    // -----------------------------------------------------
    // logicalIndex is sended only for visible columns
    // -----------------------------------------------------

    Q_D(const QpHorHeaderView);

    if (!rect.isValid())
        return;

    // get the state of the section
    QStyleOptionHeader opt;

    initStyleOption(&opt);


    QStyle::State state = QStyle::State_None;

    if (isEnabled())
        state |= QStyle::State_Enabled;

    if (window()->isActiveWindow())
        state |= QStyle::State_Active;

    if (d->clickableSections)
    {
        if (logicalIndex == d->hover)
            state |= QStyle::State_MouseOver;

        if (logicalIndex == d->pressed) // click into section
            state |= QStyle::State_Sunken;

        else if (d->highlightSelected)
        {

            if (d->sectionIntersectsSelection( logicalIndex ))
            {
                state |= QStyle::State_On;
            }


            if (d->isSectionSelected(logicalIndex))
                state |= QStyle::State_Sunken;
        }

    }

    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
    {
        opt.sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
                ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;
    }

    // setup the style options structure
    QVariant textAlignment = d->model->headerData(logicalIndex, Qt::Horizontal,
                                                  Qt::TextAlignmentRole);

    opt.rect = rect;
    opt.section = logicalIndex;
    opt.state |= state;
    opt.textAlignment = Qt::Alignment( textAlignment.isValid()
                                       ? Qt::Alignment(textAlignment.toInt())
                                       : d->defaultAlignment);

    opt.iconAlignment = Qt::AlignVCenter;

    opt.text = d->model->headerData(logicalIndex, Qt::Horizontal,
                                    Qt::DisplayRole).toString();

    if (d->textElideMode != Qt::ElideNone)
        opt.text = opt.fontMetrics.elidedText(opt.text, d->textElideMode , rect.width() - 4);

    QVariant variant = d->model->headerData(logicalIndex, Qt::Horizontal, Qt::DecorationRole);

    opt.icon = qvariant_cast<QIcon>(variant);

    if (opt.icon.isNull())
        opt.icon = qvariant_cast<QPixmap>(variant);

    QVariant foregroundBrush = d->model->headerData(logicalIndex, Qt::Horizontal,
                                                    Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
    }


    QPointF oldBO = painter->brushOrigin();

    QVariant backgroundBrush = d->model->headerData(logicalIndex, Qt::Horizontal, Qt::BackgroundRole);

    if (backgroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));

        opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));

        painter->setBrushOrigin( opt.rect.topLeft());

    }

    // the section position
    int visual = visualIndex( logicalIndex );

    //if (debug) qDebug() << "QpHorHeaderView::paintSection visual " << visual ;

    Q_ASSERT(visual != -1);

    if (count() == 1)
        opt.position = QStyleOptionHeader::OnlyOneSection;
    else if ( visual == 0)
        opt.position = QStyleOptionHeader::Beginning;
    else if ( visual == count() - 1)
        opt.position = QStyleOptionHeader::End;
    else
        opt.position = QStyleOptionHeader::Middle;

    opt.orientation = Qt::Horizontal;

    // the selected position
    bool previousSelected = d->isSectionSelected(this->logicalIndex(visual - 1));

    bool nextSelected =  d->isSectionSelected(this->logicalIndex(visual + 1));

    if (previousSelected && nextSelected)
        opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;

    else if (previousSelected)
        opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;

    else if (nextSelected)
        opt.selectedPosition = QStyleOptionHeader::NextIsSelected;

    else
        opt.selectedPosition = QStyleOptionHeader::NotAdjacent;

    // ---------------------------------------
    //           draw the section
    // ---------------------------------------


    if( logicalIndex == 4)
    {
        //opt.selectedPosition = QStyleOptionHeader::NotAdjacent;
        //painter->setFont( QFont( "Segoe UI" , painter->font().pointSize() , 50 , false));
    }

    if ( debug_paint )
    {
        qDebug() << "----------------------------------------------------------";
        qDebug() << "QpHorHeaderView::paintSection() CE_Header ";
        qDebug() << "       logicalIndex " << logicalIndex;
        qDebug() << "       opt.text " << opt.text << opt.state;
        //qDebug() << "       opt " << opt;
        //qDebug() << "       opt.selectedPosition " << dbg::toStr_SelectedPosition( opt.selectedPosition );
        //qDebug() << "       opt.position " << dbg::toStr_SectionPosition( opt.position ) << opt.position;
        //qDebug() << "       opt.section " << opt.section;
        //qDebug() << "       opt.fontMetrics.width() " << opt.fontMetrics.width("W");
        //qDebug() << "       opt.palette " << opt.palette.isBrushSet();
        //qDebug() << "       painter->fontInfo.family() " << painter->fontInfo().family();
        qDebug() << "       painter->font.weight() " << painter->font().weight();

        //        qDebug() << "  opt.palette.background() " << opt.palette.background();
        //        qDebug() << "  opt.palette.currentColorGroup(): " << dbg::toStr_ColorGroup( opt.palette.currentColorGroup());
        //        qDebug() << "  opt.palette.isBrushSet( " << opt.palette.currentColorGroup() << ")= " << opt.palette.isBrushSet( opt.palette.currentColorGroup()  , QPalette::Text);
        //        qDebug() << "  d->viewport->backgroundRole(): " << dbg::toStr_ColorRole( d->viewport->backgroundRole());

        qDebug() << "----------------------------------------------------------";
    }


    style()->drawControl(QStyle::CE_Header, &opt, painter, this);

    painter->setBrushOrigin( oldBO );
}


void QpHorHeaderView::setGridWidth(int width )
{
    Q_D( QpHorHeaderView);

    d->grid = width;
}

int QpHorHeaderView::gridWidth() const
{
    Q_D(const QpHorHeaderView);

    return d->grid;
}


QSize QpHorHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    //if (debug) qDebug()<< "QpHorHeaderView::sectionSizeFromContents";

    Q_D(const QpHorHeaderView);

    Q_ASSERT(logicalIndex >= 0);

    ensurePolished();

    // use SizeHintRole
    QVariant variant = d->model->headerData(logicalIndex, Qt::Horizontal, Qt::SizeHintRole);

    if (variant.isValid())
        return qvariant_cast<QSize>(variant);

    // otherwise use the contents
    QStyleOptionHeader opt;

    initStyleOption(&opt);

    opt.section = logicalIndex;

    QVariant var = d->model->headerData(logicalIndex, Qt::Horizontal,
                                        Qt::FontRole);
    QFont fnt;

    if (var.isValid() && var.canConvert<QFont>())
        fnt = qvariant_cast<QFont>(var);
    else
        fnt = font();

    fnt.setBold(true);
    opt.fontMetrics = QFontMetrics(fnt);

    opt.text = d->model->headerData(logicalIndex, Qt::Horizontal,
                                    Qt::DisplayRole).toString();

    variant = d->model->headerData(logicalIndex, Qt::Horizontal, Qt::DecorationRole);

    opt.icon = qvariant_cast<QIcon>(variant);

    if (opt.icon.isNull())
        opt.icon = qvariant_cast<QPixmap>(variant);

    QSize size = style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), this);

    if (isSortIndicatorShown())
    {
        int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, this);
        if (Qt::Horizontal == Qt::Horizontal)
            size.rwidth() += size.height() + margin;
        else
            size.rheight() += size.width() + margin;
    }
    return size;

}

/*!
    Returns the horizontal offset of the header. This is 0 for vertical
    headers.

    \sa offset()
*/

int QpHorHeaderView::horizontalOffset() const
{
    Q_D(const QpHorHeaderView);
    if (Qt::Horizontal == Qt::Horizontal)
        return d->offset;
    return 0;
}

int QpHorHeaderView::verticalOffset() const
{
    //    Q_D(const QpHorHeaderView);

    //    if (Qt::Horizontal == Qt::Vertical)
    //        return d->offset;

    return 0;
}

void QpHorHeaderView::updateGeometries()
{
    Q_D(QpHorHeaderView);
    d->layoutChildren();

    if (d->hasAutoResizeSections())

        d->doDelayedResizeSections();
}


int QpHorHeaderView::row_height() const
{
    Q_D(const QpHorHeaderView);

    return d->row_height();

}

int QpHorHeaderView::line_height( int line )
{
    Q_D(QpHorHeaderView);

    if( d->offsets_y.count() == 0 || line >= d->offsets_y.count())
        return -1;

    return d->offsets_y.at( line + 1 );

}




void QpHorHeaderView::scrollContentsBy(int dx, int dy)
{
    Q_D(QpHorHeaderView);

    // ----------------------------------------------------------
    //              Why this is not called?
    // ----------------------------------------------------------
    if ( debug_scroll )        qDebug() << "QpHorHeaderView::scrollContentsBy dx " << dx << "  dy " << dy;

    d->scrollDirtyRegion(dx, dy);  // set scrollDelayOffset ( this is original )
}

void QpHorHeaderView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_D(QpHorHeaderView);
    d->invalidateCachedSizeHint();
    if (d->hasAutoResizeSections()) {
        bool resizeRequired = d->globalResizeMode == ResizeToContents;
        int first = orientation() == Qt::Horizontal ? topLeft.column() : topLeft.row();
        int last = orientation() == Qt::Horizontal ? bottomRight.column() : bottomRight.row();
        for (int i = first; i <= last && !resizeRequired; ++i)
            resizeRequired = (resizeMode(i) == ResizeToContents);
        if (resizeRequired)
            d->doDelayedResizeSections();
    }
}

void QpHorHeaderView::rowsInserted(const QModelIndex &, int, int)
{
    // do nothing
}


QRect QpHorHeaderView::visualRect(const QModelIndex &) const
{
    return QRect();
}


void QpHorHeaderView::scrollTo(const QModelIndex &, ScrollHint)
{
    if ( debug_scroll )        qDebug() << "QpHorHeaderView::scrollTo do nothing";
    // do nothing - the header only displays sections
}


QModelIndex QpHorHeaderView::indexAt(const QPoint &) const
{
    return QModelIndex();
}


bool QpHorHeaderView::isIndexHidden(const QModelIndex &) const
{
    return true; // the header view has no items, just sections
}


bool QpHorHeaderView::init_sections_template( const Qp_SECTION_TMPL &matrix )
{
    Q_D(QpHorHeaderView);

    if( ! d->model)
        return false;
    /*
 visual indices :

         col0   col1   col2  col3   col4  - this virtual special columns
line0     0      1      2      2     5
line1     0      1      3      4     5
*/
    d->visual_matrix.clear();
    d->map.clear();
    d->offsets_x.clear();
    d->offsets_y.clear();
    d->sectionHidden.clear();

    d->visual_matrix = matrix ;


    d->setLines( matrix.count() );

    int max_x=0;

    int section_count_in_line=-1;

    int line=0;

    foreach( QList<int> lst , matrix)
    {
        if( lst.count() > d->model->columnCount())
        {
            qWarning() << "sections template is wrong: count of sections "<< lst.count() << " (in line " << line<< ") is more then model column count (" << d->model->columnCount() << ")";

            return false;
        }

        if( section_count_in_line >= 0 )
        {
            if( lst.count() != section_count_in_line)
                ;//return false;
        }

        max_x = qMax ( max_x , lst.count());

        line++;

    }

    int x=0;

    for( int nn=0; nn <= max_x; nn++ )
    {
        int offset = nn * QpHorHeaderView::default_section_width;
        d->offsets_x.append( offset );
    }

    if( debug_init) qDebug() << "QpHorHeaderView::init_template offsets_x " << d->offsets_x;
    //d->offsets_x << 0 << 100 << 200 << 300 <<  400 << 500  ; // количество строго  как колонки в visual_idx

    //-----------------------------------------------------------------------

    //d->offsets_y << 0 << 50 << 100 << 140;

    int y=0;

    for( int line=0; line <= matrix.count(); line++ )
    {
        d->offsets_y << line*50;
    }

    if( debug_init) qDebug() << "offsets_y " << d->offsets_y;

    if( d->offsets_y.count() - 1 != d->visual_matrix.count( ))
    {
        QMessageBox::warning( 0 ,
                              QString("error"),
                              QString("offsets_y_ size  is not equal visual_matrix lines")
                              );
    }
    //-----------------------------------------------------------------------

    d->visualColumnCount   = 5;


    int num =0;

    //int left    = visual_matrix [ line ] [ num ];

    // ---------------------------------------------------------------------
    // initializing d->map
    // ---------------------------------------------------------------------
    while( num < d->offsets_x.count() - 1 )
    {

        int line = 0;

        while( line < d->offsets_y.count() - 1 )
        {
            int val = d->visual_matrix [ line ] [ num ];

            if( d->map.contains( val ))
            {
                line++;
                continue;
            }

            qp::CELL_NODES  cell;
            cell.top        = line;
            cell.bottom     = line + 1;
            cell.left       = num;
            cell.right      = num + 1;
            cell.visible    = true;

            int ll=0;

            while ( line + ll < d->visual_matrix.count() &&  d->visual_matrix [ line + ll ] [ num ] == val )
            {
                cell.bottom = line + ll + 1;
                ll++;
            }

            int nn =0 ;

            while ( num + nn < d->visual_matrix[ line ].count() &&  d->visual_matrix [ line ] [ num + nn ] == val )
            {
                cell.right = num + nn + 1;
                nn++;
            }

            if( d->map.contains( val ))
            {

                QMessageBox::warning( 0 ,
                                      QString("error"),
                                      QString("visual index %1 already exists in section template")
                                      .arg(val)
                                      );

            }

            d->map.insert( val , cell );

            line++;
        }

        num++;
    }



    if ( debug_init )  qDebug() << "init visual_matrix : ";

    for( int line =0 ; line < d->visual_matrix.count(); ++line)
    {
        QString str;
        for( int xNum =0 ; xNum < d->visual_matrix[ line].count(); ++xNum)
        {
            str.append( QString::number( d->visual_matrix[ line] [ xNum] ) ).append(" ");
        }

        if ( debug_init )qDebug() << "line : " << line  << " : " << str;

    }

    if ( debug_init )qDebug() << "init map : ";


    int lastLogical_Num = lastLogicalNum();

    d->sectionHidden.resize( lastLogical_Num );

    // ----------------------------------------------------
    // skipped numbers add to map too and mark unvisible
    // ----------------------------------------------------


    for( int num=0; num <= lastLogical_Num; num++)
    {

        if( ! d->map.contains( num ) )
        {
            d->map.insert( num , qp::CELL_NODES());
            d->sectionHidden.setBit( num , true);
        }
        else
            if ( debug_init ) qDebug() << "num : " << num  << " : " << d->map[ num ].left << d->map[ num ].right << d->map[ num ].top << d->map[ num ].bottom << " visible:"<<d->map[ num ].visible;

    }
    /*
visual_section

  1   2   3   4   5  - xx
0  -------------------
| 0 | 1 | 2   2 | 5 |  - num
1  -------------------
| 0 | 1 | 3 | 4 | 5 |
2  -------------------
yy


*/

    //        visual_section << CELL_NODES( 0 , 1 , 0 , 2 )     // 0 num1 num2 line1 line2
    //                       << CELL_NODES( 1 , 2 , 0 , 2 )     // 1
    //                       << CELL_NODES( 2 , 4 , 0 , 1 )     // 2
    //                       << CELL_NODES( 2 , 3 , 1 , 2 )     // 3
    //                       << CELL_NODES( 3 , 4 , 1 , 2 )     // 4
    //                       << CELL_NODES( 4 , 5 , 0 , 2 )     // 5
    //                          ;
    int count=0;

    //    qDebug() << "visual_section : ";

    //    foreach( qp::CELL_NODES cell,  d->map)
    //    {
    //        d->visual_section.append( cell );
    //        qDebug() << "cell : " << count++ << " : " << cell.left << cell.right << cell. top << cell.bottom;

    //    }

    /*

the section is a area, for exapmle [ 1 ] or [ 2  2 ] or [ 5 ]
column is visual vertical area , for example
    1 or 2 or 2 or 5
    1    3    4    5
*/

    d->length_x = d->offsets_x [ d->offsets_x.count() - 1 ];

    if ( debug_init ) qDebug() << "length " << d->length_x;
    if ( debug_init ) qDebug() << "d->map.count() " << d->map.count();


    return true;

}

QModelIndex QpHorHeaderView::moveCursor(CursorAction, Qt::KeyboardModifiers)
{
    return QModelIndex();
}

void QpHorHeaderView::setSelection(const QRect&, QItemSelectionModel::SelectionFlags)
{
    if ( debug_selection ) qDebug() << "QpHorHeaderView::setSelection do nothing";
    // do nothing
}

QRegion QpHorHeaderView::visualRegionForSelection(const QItemSelection &selection) const
{

    //QAbstractItemView::visualRegionForSelection()

    Q_D(const QpHorHeaderView);

    //-------------------------------------------------
    //          for horizontal only
    //-------------------------------------------------

    const int max = d->map.count(); // d->modelSectionCount();

    int left = max;
    int right = 0;
    int rangeLeft, rangeRight;

    for (int i = 0; i < selection.count(); ++i)
    {
        QItemSelectionRange rect = selection.at(i);

        if (rect.parent().isValid() || !rect.isValid())
            continue; // we only know about toplevel items and we don't want invalid ranges

        // FIXME an item inside the range may be the leftmost or rightmost
        rangeLeft = visualIndex( rect.left() );

        if ( rangeLeft == -1) // in some cases users may change the selections
            continue;

        // before we have a chance to do the layout
        rangeRight = visualIndex(rect.right());

        if ( rangeRight == -1) // in some cases users may change the selections
            continue;         // before we have a chance to do the layout

        if (rangeLeft < left)
            left = rangeLeft;

        if (rangeRight > right)
            right = rangeRight;
    }

    int logicalLeft = logicalIndex(left);
    int logicalRight = logicalIndex(right);

    if (logicalLeft < 0  || logicalLeft >= count() ||
            logicalRight < 0 || logicalRight >= count())
    {
        qDebug() << "QpHorHeaderView::visualRegionForSelection count() " << count() << "  logicalLeft " << logicalLeft ;
        return QRegion();
    }

    int leftPos = sectionViewportPosition2( logicalLeft ).x();

    QRect rect_right =  sectionViewportPosition2( logicalRight );
    int rightPos = rect_right.x();

    rightPos += sectionSize( logicalRight );

    int hh = rect_right.height();

    QRect rect( leftPos, 0, rightPos - leftPos, hh);

    if ( debug_selection ) qDebug() << "QpHorHeaderView::visualRegionForSelection selection:" << selection << "  rect: "<<rect;

    return rect;



}


// private implementation

int QpHorHeaderViewPrivate::sectionHandleAt(const QPoint &pos, Qt::Orientation & ret)
{
    // --------------------------------------------
    //
    // --------------------------------------------

    Q_Q(QpHorHeaderView);

    int grip = q->style()->pixelMetric( QStyle::PM_HeaderGripMargin, 0, q );

    int visual = q->visualIndexAt( pos.x() , pos.y() ); // ???

    if (visual == -1)
    {
        return -1;
    }

    //qDebug() << "pos " <<  pos << " visual="<< visual;

    int lgclIdx = logicalIndex( visual );

    QRect rect = q->sectionViewportPosition2( lgclIdx );


    int x1 = rect.x() + grip;

    bool atLeft = pos.x() < x1;


    //int x2 = rect.x() + q->sectionSize( lgclIdx ) - grip ;
    int x2 = rect.x() + rect.width() - grip ;

    bool atRight =  pos.x() > x2;

    int line = q->get_section_line( pos.y() );

    if ( debug_resize ) qDebug() << "QpHorHeaderViewPrivate::sectionHandleAt pos " << pos << " line:"<< line <<" x1:"<<x1 <<" x2:"<< x2 << " rect:"<<rect << " grip " <<grip << "  lgclIdx:"<<lgclIdx;


    int y1 = rect.y() + grip;

    bool atTop = pos.y() < y1;


    int y2 = rect.y() + rect.height() - grip;

    bool atBottom = pos.y() > y2;

    //qDebug() << "QpHorHeaderViewPrivate::sectionHandleAt pos.y() " << pos.y() << " line:"<< line <<" y1:"<<y1<< " rect.y():"<<rect.y() << " grip " <<grip << "  lgclIdx:"<<lgclIdx;

    if( atLeft)
        if ( debug_resize ) qDebug() << "atLeft";

        else if( atRight)
            if ( debug_resize ) qDebug() << "atRight";

            else if( atBottom)
                if ( debug_resize ) qDebug() << "atBottom";

                else if( atTop)
                    if ( debug_resize ) qDebug() << "atTop";
                    else
                        if ( debug_resize ) qDebug() << "nothing";

    if (atLeft)
    {
        //grip at the beginning of the section
        while(visual > -1)
        {
            if( ! map.contains( visual) )
                return -1;

            int num1 = map[ visual ].left;

            if( num1  == 0)
                return -1;

            int visual = visual_matrix [ line ] [ num1 -1 ];

            return visual;

            //            int logical = q->logicalIndex( --visual );

            //            if ( ! q->isSectionHidden(logical))
            //            {
            //                if ( debug ) qDebug() << "atLeft       sectionHandleAt=" << logical;

            //                ret = Qt::Horizontal;
            //                return logical;
            //            }
        }
    }
    else if (atRight)
    {
        if ( debug ) qDebug() << "atRight       sectionHandleAt=" << lgclIdx;

        ret = Qt::Horizontal;

        return lgclIdx;
    }
    else if ( atTop )
    {
        //int line = q->get_section_line( pos.y());

        if( line <= 0 ) // top line top border - dont resize
            return -1;

        int num = q->get_section_num( pos.x());

        visual = visual_matrix [ line - 1 ] [ num ];

        int lgclIdx = q->logicalIndex( visual );

        if ( debug ) qDebug() << "atTop       sectionHandleAt=" << lgclIdx;


        ret = Qt::Vertical;
        return lgclIdx;
    }
    else if ( atBottom )
    {
        if ( debug ) qDebug() << "atBottom       sectionHandleAt=" << lgclIdx;

        ret = Qt::Vertical;
        return lgclIdx;
    }


    return -1;
}

//int QpHorHeaderViewPrivate::sectionHandleAt_forCursorMoving(const QPoint &pos, const QPoint &lastCursor, Qt::Orientation & ret)
//{
//    // --------------------------------------------
//    //
//    // --------------------------------------------

//    Q_Q(QpHorHeaderView);

//    Qt::Orientation orientation;

//    if( sectionHandleAt (pos , orientation) == -1 )
//        return -1;

//    int grip = q->style()->pixelMetric( QStyle::PM_HeaderGripMargin, 0, q );

//    int toTop = 0;
//    int toLeft = 0;
//    int toRight = 0;
//    int toBottom = 0;

//    if( pos.x() < lastCursor.x())
//    {
//        toLeft = lastCursor.x() - pos.x();
//    }

//    if( pos.x() > lastCursor.x())
//    {
//        toRight = pos.x() - lastCursor.x() ;
//    }

//    if( pos.y() < lastCursor.y())
//    {
//        toTop = lastCursor.y() - pos.y() ;
//    }

//    if( pos.y() > lastCursor.y())
//    {
//        toBottom = pos.y() - lastCursor.y();
//    }

//    qDebug() << "   toLeft:"<<toLeft;
//    qDebug() << "   toRight:"<<toRight;
//    qDebug() << "   toTop:"<<toTop;
//    qDebug() << "   toBottom:"<<toBottom;


//    int visual = q->visualIndexAt( pos.x() , pos.y() ); // ???


//    if (visual == -1)
//    {
//        //qDebug() << "?????????? pos " <<  pos << " lastCursor " << lastCursor << " ? visual="<< visual;
//        return -1;
//    }

//    int visual2 =-1;

//    qDebug() << "pos " <<  pos << " lastCursor " << lastCursor << " visual="<< visual;

//    if( toLeft > toTop && toLeft > toBottom ) //
//    {
//        visual2 = q->visualIndexAt( pos.x() - grip , pos.y() );

//        if( visual2 != visual)
//            visual = visual2;

//        ret = Qt::Horizontal;

//    }

//    else if( toRight > toTop && toRight > toBottom) //
//    {
//        visual2 = q->visualIndexAt( pos.x() - grip, pos.y()  );

//        if( visual2 != visual)
//            visual = visual2;

//        ret = Qt::Horizontal;
//    }

//    else if( toTop > toLeft && toTop > toRight) //
//    {
//        visual2 = q->visualIndexAt( pos.x() , pos.y() - grip );

//        if( visual2 != visual)
//            visual = visual2;

//        ret = Qt::Vertical;
//    }

//    else if( toBottom > toLeft && toBottom > toRight) //
//    {
//        visual2 = q->visualIndexAt( pos.x() , pos.y() - grip );

//        if( visual2 != visual)
//            visual = visual2;

//        ret = Qt::Vertical;
//    }


//    int lgclIdx = logicalIndex( visual );

//    if( lgclIdx == -1)
//        return -1;

//    QRect rect = q->sectionViewportPosition2( lgclIdx );

//    return lgclIdx;

//}

void QpHorHeaderViewPrivate::setupSectionIndicator(int section, int position)
{
    Q_Q(QpHorHeaderView);
    if (!sectionIndicator)
    {
        sectionIndicator = new QLabel(viewport);
    }

    int w, h;
    int p = q->sectionViewportPosition2(section).x();

    w = q->sectionSize(section);
    h = viewport->height();

    sectionIndicator->resize(w, h);

    QPixmap pm(w, h);
    pm.fill(QColor(0, 0, 0, 45));
    QRect rect(0, 0, w, h);

    QPainter painter(&pm);
    painter.setOpacity(0.75);
    q->paintSection(&painter, rect, section);
    painter.end();

    sectionIndicator->setPixmap(pm);
    sectionIndicatorOffset = position - qMax(p, 0);
}

void QpHorHeaderViewPrivate::updateSectionIndicator(int section, int position)
{
    if (!sectionIndicator)
        return;

    if (section == -1 || target == -1) {
        sectionIndicator->hide();
        return;
    }

    sectionIndicator->move(position - sectionIndicatorOffset, 0);

    sectionIndicator->show();
}

/*!
    Initialize \a option with the values from this QpHorHeaderView. This method is
    useful for subclasses when they need a QStyleOptionHeader, but do not want
    to fill in all the information themselves.

    \sa QStyleOption::initFrom()
*/
void QpHorHeaderView::initStyleOption(QStyleOptionHeader *option) const
{
    Q_D(const QpHorHeaderView);
    option->initFrom(this);
    option->state = QStyle::State_None | QStyle::State_Raised;

    option->orientation = Qt::Horizontal;

    option->state |= QStyle::State_Horizontal;
    option->section = 0;
}

bool QpHorHeaderViewPrivate::isSectionSelected(int section) const
{
    int i = section * 2;
    if (i < 0 || i >= sectionSelected.count())
        return false;
    if (sectionSelected.testBit(i)) // if the value was cached
        return sectionSelected.testBit(i + 1);
    bool s = false;

    s = isColumnSelected(section);

    sectionSelected.setBit(i + 1, s); // selection state
    sectionSelected.setBit(i, true); // cache state
    return s;
}

/*!
    \internal
    Returns the last visible (ie. not hidden) visual index
*/
int QpHorHeaderViewPrivate::lastVisibleVisualIndex() const
{
    Q_Q(const QpHorHeaderView);
    for (int visual = q->count()-1; visual >= 0; --visual) {
        if (!q->isSectionHidden(q->logicalIndex(visual)))
            return visual;
    }

    //default value if no section is actually visible
    return -1;
}

/*!
    \internal
    Go through and resize all of the sections applying stretchLastSection,
    manualy stretches, sizes, and useGlobalMode.

    The different resize modes are:
    Interactive - the user decides the size
    Stretch - take up whatever space is left
    Fixed - the size is set programmatically outside the header
    ResizeToContentes - the size is set based on the contents of the row or column in the parent view

    The resize mode will not affect the last section if stretchLastSection is true.
*/

void QpHorHeaderViewPrivate::resizeSections(QpHorHeaderView::ResizeMode globalMode, bool useGlobalMode)
{
    Q_Q(QpHorHeaderView);


    if ( debug )        qDebug() << "QpHorHeaderViewPrivate::resizeSections globalMode:"<<globalMode<< "  useGlobalMode:"<<useGlobalMode ;

    //  stop the timer in case it is delayed
    delayedResize.stop();

    executePostedLayout();

    if (sectionCount == 0)
        return;

    if (resizeRecursionBlock)
        return;

    resizeRecursionBlock = true;

    invalidateCachedSizeHint();

    const int lastVisibleSection = lastVisibleVisualIndex();

    // find stretchLastSection if we have it
    int stretchSection = -1;

    if (stretchLastSection && !useGlobalMode)
        stretchSection = lastVisibleVisualIndex();

    // count up the number of strected sections and how much space left for them
    int lengthToStrech = viewport->width();
    int numberOfStretchedSections = 0;

    QList<int> section_sizes;

    for (int i = 0; i < sectionCount; ++i)
    {
        if (isVisualIndexHidden(i))
            continue;

        QpHorHeaderView::ResizeMode resizeMode;

        if (useGlobalMode && (i != stretchSection))
            resizeMode = globalMode;
        else
            resizeMode = (i == stretchSection ? QpHorHeaderView::Stretch : headerSectionResizeMode(i));

        if (resizeMode == QpHorHeaderView::Stretch)
        {
            ++numberOfStretchedSections;
            section_sizes.append(headerSectionSize(i));
            continue;
        }

        // because it isn't stretch, determine its width and remove that from lengthToStrech
        int sectionSize = 0;

        if (resizeMode == QpHorHeaderView::Interactive || resizeMode == QpHorHeaderView::Fixed)
        {
            sectionSize = headerSectionSize(i);
        }
        else
        { // resizeMode == QpHorHeaderView::ResizeToContents
            int logicalIndex = q->logicalIndex(i);
            sectionSize = qMax(viewSectionSizeHint(logicalIndex),
                               q->sectionSizeHint(logicalIndex));
        }

        section_sizes.append(sectionSize);
        lengthToStrech -= sectionSize;
    }

    // calculate the new length for all of the stretched sections
    int stretchSectionLength = -1;
    int pixelReminder = 0;

    if (numberOfStretchedSections > 0 && lengthToStrech > 0)
    { // we have room to stretch in
        int hintLengthForEveryStretchedSection = lengthToStrech / numberOfStretchedSections;
        stretchSectionLength = qMax(hintLengthForEveryStretchedSection, q->minimumSectionSize());
        pixelReminder = lengthToStrech % numberOfStretchedSections;
    }

    int spanStartSection = 0;
    int previousSectionLength = 0;

    QpHorHeaderView::ResizeMode previousSectionResizeMode = QpHorHeaderView::Interactive;

    // resize each section along the total length
    for (int i = 0; i < sectionCount; ++i)
    {

        //        int oldSectionLength = headerSectionSize(i);
        //        int newSectionLength = -1;

        //        QpHorHeaderView::ResizeMode newSectionResizeMode = headerSectionResizeMode(i);

        //        if (isVisualIndexHidden(i))
        //        {
        //            newSectionLength = 0;
        //        }
        //        else
        //        {
        //            QpHorHeaderView::ResizeMode resizeMode;

        //            if (useGlobalMode)
        //                resizeMode = globalMode;
        //            else
        //                resizeMode = (i == stretchSection
        //                              ? QpHorHeaderView::Stretch
        //                              : newSectionResizeMode);
        //            if (resizeMode == QpHorHeaderView::Stretch && stretchSectionLength != -1)
        //            {
        //                if (i == lastVisibleSection)
        //                    newSectionLength = qMax(stretchSectionLength, lastSectionSize);
        //                else
        //                    newSectionLength = stretchSectionLength;

        //                if (pixelReminder > 0)
        //                {
        //                    newSectionLength += 1;
        //                    --pixelReminder;
        //                }
        //                section_sizes.removeFirst();
        //            }
        //            else
        //            {
        //                newSectionLength = section_sizes.front();
        //                section_sizes.removeFirst();
        //            }
        //        }

        //        //Q_ASSERT(newSectionLength > 0);
        //        if ((previousSectionResizeMode != newSectionResizeMode
        //             || previousSectionLength != newSectionLength) && i > 0)
        //        {
        //            int spanLength = (i - spanStartSection) * previousSectionLength;

        //            createSectionSpan(spanStartSection, i - 1, spanLength, previousSectionResizeMode);

        //            //Q_ASSERT(headerLength() == length);
        //            spanStartSection = i;
        //        }

        //        if (newSectionLength != oldSectionLength)
        //            emit q->sectionResized(logicalIndex(i), oldSectionLength, newSectionLength);

        //        previousSectionLength = newSectionLength;
        //        previousSectionResizeMode = newSectionResizeMode;
    }

    //Q_ASSERT(headerLength() == length);
    resizeRecursionBlock = false;
    viewport->update();
}


void QpHorHeaderViewPrivate::clear()
{
    if (state != NoClear)
    {
        if(length_x = offsets_x.count() >0)
            length_x =  offsets_x [ offsets_x.count() - 1 ];
        else
            length_x = 0;

        sectionCount = 0;
        visualIndices.clear();
        logicalIndices.clear();
        sectionSelected.clear();
        sectionHidden.clear();
        hiddenSectionSize.clear();
        //sectionSpans.clear();
    }
}

void QpHorHeaderViewPrivate::flipSortIndicator(int section)
{
    Q_Q(QpHorHeaderView);
    Qt::SortOrder sortOrder;

    if (sortIndicatorSection == section)
    {
        sortOrder = (sortIndicatorOrder == Qt::DescendingOrder) ? Qt::AscendingOrder : Qt::DescendingOrder;
    }
    else
    {
        const QVariant value = model->headerData(section, Qt::Horizontal, Qt::InitialSortOrderRole);
        if (value.canConvert(QVariant::Int))
            sortOrder = static_cast<Qt::SortOrder>(value.toInt());
        else
            sortOrder = Qt::AscendingOrder;
    }

    q->setSortIndicator(section, sortOrder);
}

void QpHorHeaderViewPrivate::cascadingResize(int visual, int newSize)
{
    Q_Q(QpHorHeaderView);
    const int minimumSize = q->minimumSectionSize();
    //    const int oldSize = headerSectionSize(visual);
    //    int delta = newSize - oldSize;

    //    if (delta > 0) { // larger
    //        bool sectionResized = false;

    //        // restore old section sizes
    //        for (int i = firstCascadingSection; i < visual; ++i) {
    //            if (cascadingSectionSize.contains(i)) {
    //                int currentSectionSize = headerSectionSize(i);
    //                int originalSectionSize = cascadingSectionSize.value(i);
    //                if (currentSectionSize < originalSectionSize) {
    //                    int newSectionSize = currentSectionSize + delta;
    //                    resizeSectionSpan(i, currentSectionSize, newSectionSize);
    //                    if (newSectionSize >= originalSectionSize && false)
    //                        cascadingSectionSize.remove(i); // the section is now restored
    //                    sectionResized = true;
    //                    break;
    //                }
    //            }

    //        }

    //        // resize the section
    //        if (!sectionResized) {
    //            newSize = qMax(newSize, minimumSize);
    //            if (oldSize != newSize)
    //                resizeSectionSpan(visual, oldSize, newSize);
    //        }

    //        // cascade the section size change
    //        for (int i = visual + 1; i < sectionCount; ++i) {
    //            if (!sectionIsCascadable(i))
    //                continue;
    //            int currentSectionSize = headerSectionSize(i);
    //            if (currentSectionSize <= minimumSize)
    //                continue;
    //            int newSectionSize = qMax(currentSectionSize - delta, minimumSize);
    //            //qDebug() << "### cascading to" << i << newSectionSize - currentSectionSize << delta;
    //            resizeSectionSpan(i, currentSectionSize, newSectionSize);
    //            saveCascadingSectionSize(i, currentSectionSize);
    //            delta = delta - (currentSectionSize - newSectionSize);
    //            //qDebug() << "new delta" << delta;
    //            //if (newSectionSize != minimumSize)
    //            if (delta <= 0)
    //                break;
    //        }
    //    } else { // smaller
    //        bool sectionResized = false;

    //        // restore old section sizes
    //        for (int i = lastCascadingSection; i > visual; --i) {
    //            if (!cascadingSectionSize.contains(i))
    //                continue;
    //            int currentSectionSize = headerSectionSize(i);
    //            int originalSectionSize = cascadingSectionSize.value(i);
    //            if (currentSectionSize >= originalSectionSize)
    //                continue;
    //            int newSectionSize = currentSectionSize - delta;
    //            resizeSectionSpan(i, currentSectionSize, newSectionSize);
    //            if (newSectionSize >= originalSectionSize && false) {
    //                //qDebug() << "section" << i << "restored to" << originalSectionSize;
    //                cascadingSectionSize.remove(i); // the section is now restored
    //            }
    //            sectionResized = true;
    //            break;
    //        }

    //        // resize the section
    //        resizeSectionSpan(visual, oldSize, qMax(newSize, minimumSize));

    //        // cascade the section size change
    //        if (delta < 0 && newSize < minimumSize) {
    //            for (int i = visual - 1; i >= 0; --i) {
    //                if (!sectionIsCascadable(i))
    //                    continue;
    //                int sectionSize = headerSectionSize(i);
    //                if (sectionSize <= minimumSize)
    //                    continue;
    //                resizeSectionSpan(i, sectionSize, qMax(sectionSize + delta, minimumSize));
    //                saveCascadingSectionSize(i, sectionSize);
    //                break;
    //            }
    //        }

    //        // let the next section get the space from the resized section
    //        if (!sectionResized) {
    //            for (int i = visual + 1; i < sectionCount; ++i) {
    //                if (!sectionIsCascadable(i))
    //                    continue;
    //                int currentSectionSize = headerSectionSize(i);
    //                int newSectionSize = qMax(currentSectionSize - delta, minimumSize);
    //                resizeSectionSpan(i, currentSectionSize, newSectionSize);
    //                break;
    //            }
    //        }
    //    }

    //    if (hasAutoResizeSections())
    //        doDelayedResizeSections();

    //    viewport->update();
}

void QpHorHeaderViewPrivate::setDefaultSectionSize(int size)
{
    Q_Q(QpHorHeaderView);
    defaultSectionSize = size;
    //int currentVisualIndex = 0;


    //    for (int i = 0; i < sectionSpans.count(); ++i)
    //    {
    //        QpHorHeaderViewPrivate::SectionSpan &span = sectionSpans[i];

    //        if (span.size > 0)
    //        {
    //            //we resize it if it is not hidden (ie size > 0)
    //            const int newSize = span.count * size;

    //            if (newSize != span.size)
    //            {
    //                length += newSize - span.size; //the whole length is changed
    //                const int oldSectionSize = span.sectionSize();
    //                span.size = span.count * size;
    //                for (int i = currentVisualIndex; i < currentVisualIndex + span.count; ++i) {
    //                    emit q->sectionResized(logicalIndex(i), oldSectionSize, size);
    //                }
    //            }
    //        }
    //        currentVisualIndex += span.count;
    //    }
}

//const int QpHorHeaderViewPrivate::headerSectionNumber( int visual ) const
//{
//    if( visual <0 )
//        return -1;

//    const QRect rect = headerSectionPosition2( visual );

//}



int QpHorHeaderViewPrivate::headerSectionSize(int visual) const
{
    //Q_ASSERT ( 1==0);

    QRect rect = headerSectionPosition2 ( visual );

    int x1 = rect.left();
    int x2 = rect.right();

    int width = rect.width() ; // -1 this is so strange

    return width;

    //    // ### stupid iteration
    //    int section_start = 0;

    //    const int sectionSpansCount = sectionSpans.count();

    //    for (int i = 0; i < sectionSpansCount; ++i)
    //    {
    //        const QpHorHeaderViewPrivate::SectionSpan &currentSection = sectionSpans.at(i);

    //        int section_end = section_start + currentSection.count - 1;

    //        if (visual >= section_start && visual <= section_end)

    //            return currentSection.sectionSize();

    //        section_start = section_end + 1;
    //    }
    return -1;
}

void QpHorHeaderViewPrivate::setHeaderSectionSize( int visual, int newWidth)
{
    if(visual <0 || visual >= map.count())
        return;

    const qp::CELL_NODES &cell = map[ visual ];

    int left    = cell.left;
    int right   = cell.right ;
    int top     = cell.top;
    int bottom  = cell.bottom;

    int x1 = offsets_x[ left ];
    int y1 = offsets_y[ top ];

    int x2 = offsets_x[ right ];
    int y2 = offsets_y[ bottom ];

    QPoint tl( x1 , y1 );
    QPoint rb( x2 , y2 );

    QRect rect ( tl , rb );

    int oldWidth = x2 - x1;

    int add = newWidth - oldWidth;

    if(add == 0)
        return;

    int col = right;

    while (col < offsets_x.count() )
    {
        int xx = offsets_x[ col ] + add;
        offsets_x[ col ] = xx;
        col++;
    }

    length_x = offsets_x[ offsets_x.count() - 1 ];

    if ( debug ) qDebug() << " QpHorHeaderViewPrivate::setHeaderSectionSize ";
    if ( debug ) qDebug() << "   offsets_x_ " << offsets_x;

}


const QRect QpHorHeaderViewPrivate::headerSectionPosition2(int logicalColNum) const
{

    int count = model->columnCount();

    if( ! map.contains( logicalColNum ) || ! map[ logicalColNum ].visible )

        return QRect();

    const qp::CELL_NODES &cell = map[ logicalColNum ];

    int left    = cell.left;
    int right   = cell.right ;
    int top     = cell.top;
    int bottom  = cell.bottom;

    int x1 = offsets_x[ left ];
    int y1 = offsets_y[ top ];

    int x2 = offsets_x[ right ];
    int y2 = offsets_y[ bottom ];

    QPoint tl( x1 , y1 );
    QPoint rb( x2 , y2 );

    QRect rect ( tl , rb ) ;

    return rect;
}

const QRect QpHorHeaderViewPrivate::headerSectionPosition3(int visual) const
{
    /*
     visual indices :

    line0     0      1      2      2     5
    line1     0      1      3      4     5
             col0   col1   col2  col3   col4  - this virtual special columns
    */
    //    int count = model->columnCount();

    //    if( visual >= count || visual < 0 )

    //        return QRect();

    //    if( offsets_y.count() == 0)
    //        return QRect();

    //    // ---------------------------------------
    //    //              find
    //    // ---------------------------------------
    //    //int col = offsets_x.count()-1;

    //    // offsets_x.count() may be less then model->columnCount()
    //    int columnCount = model->columnCount()-1;
    //    int arrCount = offsets_x_.count()-1;

    //    int col = qMin( arrCount , columnCount );

    //    int right   = -1;
    //    int left    = -1;
    //    int top     = -1;
    //    int bottom  = -1;

    //    while( col >=0  )
    //    {

    //        top     = 0 ;

    //        for( int line = 0; line < offsets_y.count(); line++)
    //        {
    //            int idx2 = visual_matrix [ line ] [ col ];

    //            if ( idx2 == visual)
    //            {
    //                right   = offsets_x [ col ];
    //                bottom  = offsets_y [ line ];

    //                if( line > 0)
    //                    top  = offsets_y [ line - 1 ];

    //                /*
    //                    line0     0      1      2      2     5
    //                    line1     0      1      3      4     5
    //                             col0   col1   col2  col3   col4  - this virtual special columns
    //                */

    //                // ---------------------------------------------------
    //                //      find to the bottom a index equal visual
    //                // ---------------------------------------------------

    //                int lineToTheBottom = line + 1;

    //                while ( lineToTheBottom <  offsets_y.count() && visual == visual_matrix [ lineToTheBottom ] [ col ]) // navigate to bottom
    //                {
    //                    lineToTheBottom++;
    //                }

    //                bottom  = offsets_y [ lineToTheBottom -1 ];


    //                // ---------------------------------------------------
    //                //      find to the left a index equal visual
    //                // ---------------------------------------------------

    //                /*
    //                line0     0      1      3      4     5
    //                line1     1      2      3      4     5
    //                */

    //                if( col == 0)
    //                    left = 0;
    //                else
    //                {
    //                    int colToTheLeft = col-1;

    //                    while ( colToTheLeft >= 0 && visual ==  visual_matrix [ line ] [ colToTheLeft ]) // navigate to left
    //                    {
    //                        colToTheLeft--;
    //                    }

    //                    if( colToTheLeft == -1)
    //                        left = 0;
    //                    else
    //                        left = offsets_x [ colToTheLeft ];


    //                }

    //                //  left, top, right, bottom was been known

    //                //                if( debug_p)
    //                //                    qDebug() << "QpHorHeaderViewPrivate 2 visual : " <<  visual <<" left, top, right, bottom : " << left << top << right << bottom;

    //                QPoint lt( left + grid , top );
    //                QPoint rb( right ,  bottom - grid );

    //                return QRect ( lt , rb );
    //                //return left;

    //                break;

    //            }
    //        }

    //        col--;
    //    }

    return QRect();
}


int QpHorHeaderViewPrivate::headerVisualIndexAt2(int x , int line) const
{
    // x - coordinate in viewport

    //if (debug) qDebug() << " QpHorHeaderViewPrivate::headerVisualIndexAt " << x << " line " << line;


    if( line < 0 )
        return -1;

    int count = offsets_x.count();

    if( count == 0 )
        return -1;

    int max_x = offsets_x.at( count -1 );


    if(offset > 0)
        x += offset; // !!

    if( x >= max_x )
        return -1; // when out of table area

    for( int col = 1; col < offsets_x.count() ; col++ )
    {
        int xx = offsets_x[ col ];

        if( x < xx )
        {
            return visual_matrix [ line ] [ col - 1 ] ;
        }
    }

    return -1;
}

void QpHorHeaderViewPrivate::setHeaderSectionResizeMode(int visual, QpHorHeaderView::ResizeMode mode)
{
    int size = headerSectionSize(visual);
    //createSectionSpan(visual, visual, size, mode);
}

QpHorHeaderView::ResizeMode QpHorHeaderViewPrivate::headerSectionResizeMode(int visual) const
{
    //    int span = sectionSpanIndex(visual);
    //    if (span == -1)
    return globalResizeMode;
    //    return sectionSpans.at(span).resizeMode;
}

void QpHorHeaderViewPrivate::setGlobalHeaderResizeMode(QpHorHeaderView::ResizeMode mode)
{
    globalResizeMode = mode;
    //    for (int i = 0; i < sectionSpans.count(); ++i)
    //        sectionSpans[i].resizeMode = mode;
}

int QpHorHeaderViewPrivate::viewSectionSizeHint(int logical) const
{
    if (QAbstractItemView *view = qobject_cast<QAbstractItemView*>(parent))
    {
        return  view->sizeHintForColumn(logical);
    }
    return 0;
}

int QpHorHeaderViewPrivate::adjustedVisualIndex(int visualIndex) const
{
    if (hiddenSectionSize.count() > 0)
    {
        int adjustedVisualIndex = visualIndex;
        int currentVisualIndex = 0;


        visualIndex = adjustedVisualIndex;
    }
    //        for (int i = 0; i < sectionSpans.count(); ++i)
    //        {
    //            if (sectionSpans.at(i).size == 0)
    //                adjustedVisualIndex += sectionSpans.at(i).count;
    //            else
    //                currentVisualIndex += sectionSpans.at(i).count;
    //            if (currentVisualIndex >= visualIndex)
    //                break;
    //        }

    return visualIndex;
}

#ifndef QT_NO_DATASTREAM
void QpHorHeaderViewPrivate::write(QDataStream &out) const
{
    //out << int(orientation);
    out << int(sortIndicatorOrder);
    out << sortIndicatorSection;
    out << sortIndicatorShown;

    out << visualIndices;
    out << logicalIndices;

    out << sectionHidden;
    out << hiddenSectionSize;

    out << length_x;
    out << sectionCount;
    out << movableSections;
    out << clickableSections;
    out << highlightSelected;
    out << stretchLastSection;
    out << cascadingResizing;
    out << stretchSections;
    out << contentsSections;
    out << defaultSectionSize;
    out << minimumSectionSize;

    out << int(defaultAlignment);
    out << int(globalResizeMode);

    //out << sectionSpans;
}

bool QpHorHeaderViewPrivate::read(QDataStream &in)
{
    int orient, order, align, global;
    //    in >> orient;
    //    orientation = (Qt::Orientation)orient;

    in >> order;
    sortIndicatorOrder = (Qt::SortOrder)order;

    in >> sortIndicatorSection;
    in >> sortIndicatorShown;

    in >> visualIndices;
    in >> logicalIndices;

    in >> sectionHidden;
    in >> hiddenSectionSize;

    in >> length_x;
    in >> sectionCount;
    in >> movableSections;
    in >> clickableSections;
    in >> highlightSelected;
    in >> stretchLastSection;
    in >> cascadingResizing;
    in >> stretchSections;
    in >> contentsSections;
    in >> defaultSectionSize;
    in >> minimumSectionSize;

    in >> align;
    defaultAlignment = Qt::Alignment(align);

    in >> global;
    globalResizeMode = (QpHorHeaderView::ResizeMode)global;

    //in >> sectionSpans;

    return true;
}

#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE

#endif // QT_NO_ITEMVIEWS

#include "moc_qp_horheaderview.cpp"