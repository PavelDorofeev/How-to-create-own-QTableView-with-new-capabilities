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

#include "qp_vertheaderview.h"

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

#include "qp/tableview/qp_vertheaderview_p.h"
#include <private/qabstractitemmodel_p.h>
#include "qp_horheaderview.h"

#ifndef QT_NO_DATASTREAM
#include <qdatastream.h>
#endif

QT_BEGIN_NAMESPACE


const int QpVertHeaderViewPrivate::default_section_height = 50;

const  bool QpVertHeaderView::debug_paint = true;
const  bool QpVertHeaderView::debug_select = false;
const  bool QpVertHeaderView::debug_init = true;
const  bool QpVertHeaderView::debug_scroll = false;
const  bool QpVertHeaderView::debug_size = true;
const  bool QpVertHeaderView::debug_mdl_signals = true;

const  bool QpVertHeaderViewPrivate::debug_select = false;
const  bool QpVertHeaderViewPrivate::debug = false;
const  bool QpVertHeaderViewPrivate::debug_size = true;
const  bool QpVertHeaderViewPrivate::debug_mdl_signals = true;


QpVertHeaderView::QpVertHeaderView( const QpHorHeaderView &Horizontal,
                                    //Qt::Orientation orientation,
                                    QWidget *parent)
    :
      QpAbstractItemView(*new QpVertHeaderViewPrivate( Horizontal), parent)
{
    Q_D(QpVertHeaderView);


    d->setDefaultValues( );

    init();

    Q_ASSERT ( connect ( &Horizontal , SIGNAL(sig_sectionsTmplChanged()),
                         this , SLOT( slot_sectionsTmplChanged() ) ) == true) ;
}


QpVertHeaderView::QpVertHeaderView(QpVertHeaderViewPrivate &dd,
                                   //Qt::Orientation orientation,
                                   QWidget *parent)
    : QpAbstractItemView(dd, parent)
{
    Q_D(QpVertHeaderView);

    d->setDefaultValues();

    init();
}

QpVertHeaderView::~QpVertHeaderView()
{
}


void QpVertHeaderView::init()
{
    Q_D(QpVertHeaderView);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFrameStyle(NoFrame);
    setFocusPolicy(Qt::NoFocus);
    d->viewport->setMouseTracking(true);
    d->viewport->setBackgroundRole(QPalette::Button);
    d->textElideMode = Qt::ElideNone;

    delete d->itemDelegate;
}


void QpVertHeaderView::setModel(QAbstractItemModel *model)
{
    if(debug_init) qDebug() << "QpVertHeaderView::setModel";

    if (model == this->model())
        return;

    Q_D(QpVertHeaderView);

    if (d->model && d->model != QAbstractItemModelPrivate::staticEmptyModel())
    {

        Q_ASSERT ( QObject::disconnect(d->model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                                       this, SLOT(sectionsInserted(QModelIndex,int,int)))== true);

        Q_ASSERT ( QObject::disconnect(d->model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                                       this, SLOT(sectionsAboutToBeRemoved(QModelIndex,int,int)))== true);

        Q_ASSERT ( QObject::disconnect(d->model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                                       this, SLOT(_q_sectionsRemoved(QModelIndex,int,int)))== true);

        Q_ASSERT ( QObject::disconnect(d->model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
                                       this, SLOT(headerDataChanged(Qt::Orientation,int,int)))== true);

        Q_ASSERT ( QObject::disconnect(d->model, SIGNAL(layoutAboutToBeChanged()),
                                       this, SLOT(_q_layoutAboutToBeChanged()))== true);
    }

    if (model && model != QAbstractItemModelPrivate::staticEmptyModel())
    {

        if(debug_init) qDebug() << "QpVertHeaderView::setModel QObject::connect";

        Q_ASSERT (QObject::connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
                                   this, SLOT(sectionsInserted(QModelIndex,int,int))) == true);

        Q_ASSERT (QObject::connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                                   this, SLOT(sectionsAboutToBeRemoved(QModelIndex,int,int))) == true);

        Q_ASSERT (QObject::connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                                   this, SLOT(_q_sectionsRemoved(QModelIndex,int,int)))== true);

        Q_ASSERT (QObject::connect(model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
                                   this, SLOT(headerDataChanged(Qt::Orientation,int,int)))== true);

        Q_ASSERT (QObject::connect(model, SIGNAL(layoutAboutToBeChanged()),
                                   this, SLOT(_q_layoutAboutToBeChanged())) == true);
    }

    d->state = QpVertHeaderViewPrivate::NoClear;
    QpAbstractItemView::setModel(model);
    d->state = QpVertHeaderViewPrivate::NoState;

    // Users want to set sizes and modes before the widget is shown.
    // Thus, we have to initialize when the model is set,
    // and not lazily like we do in the other views.
    initializeSections();

}



int QpVertHeaderView::offset() const
{
    Q_D(const QpVertHeaderView);

    if ( debug_size ) qDebug() << "QpVertHeaderView::offset() : "<<d->offset;

    return d->offset;
}


void QpVertHeaderView::setOffset(int newOffset)
{
    Q_D(QpVertHeaderView);

    if ( debug_scroll ) qDebug() << "QpVertHeaderView::setOffset("<<newOffset;

    if (d->offset == (int)newOffset)
        return;

    int ndelta = d->offset - newOffset;

    d->offset = newOffset;

    d->viewport->scroll(0, ndelta);

    if (d->state == QpVertHeaderViewPrivate::ResizeSection)
    {
        QPoint cursorPos = QCursor::pos();

        QCursor::setPos(cursorPos.x(), cursorPos.y() + ndelta);

        d->firstPos += ndelta;
        d->lastPos += ndelta;
    }
}

void QpVertHeaderView::setOffsetToSectionPosition(int visualIndex)
{
    Q_D(QpVertHeaderView);
    if (visualIndex > -1 && visualIndex < d->sectionCount)
    {
        int position = d->headerSectionPosition_Y(d->adjustedVisualIndex(visualIndex));

        setOffset(position);
    }
}


void QpVertHeaderView::setOffsetToLastSection()
{
    Q_D(const QpVertHeaderView);

    int size =  viewport()->height();

    int len = length();
    int position = len - size;

    setOffset(position);

    if( debug_paint ) qDebug() << " QpVertHeaderView::setOffsetToLastSection() position " << position
                               << " size :"<<size;
}

int QpVertHeaderView::length() const
{
    Q_D(const QpVertHeaderView);


    //qDebug() << "QpVertHeaderView::length() count() " << count() << " * row_heigth() " <<row_heigth()<< " = " << ll ;

    if( debug_paint ) qDebug() << "QpVertHeaderView::length()  d->length" << d->length << " d->sectionCount" <<d->sectionCount;


    return d->length;  // y

}

QSize QpVertHeaderView::sizeHint() const
{
    Q_D(const QpVertHeaderView);

    if (d->cachedSizeHint.isValid())
        return d->cachedSizeHint;

    d->cachedSizeHint = QSize(0, 0); //reinitialize the cached size hint

    const int section_Count = count();

    // get size hint for the first n sections
    int i = 0;

    for (int checked = 0; checked < 100 && i < section_Count; ++i)
    {
        if (isSectionHidden(i))
            continue;
        checked++;
        QSize hint = sectionSizeFromContents(i);
        d->cachedSizeHint = d->cachedSizeHint.expandedTo(hint);
    }

    // get size hint for the last n sections
    i = qMax(i, section_Count - 100 );

    for (int j = section_Count - 1, checked = 0; j >= i && checked < 100; --j)
    {
        if (isSectionHidden(j))
            continue;
        checked++;
        QSize hint = sectionSizeFromContents(j);

        d->cachedSizeHint = d->cachedSizeHint.expandedTo(hint);
    }

    if(debug_size) qDebug() << "QpVertHeaderView::sizeHint() = " << d->cachedSizeHint;

    return d->cachedSizeHint;
}

/*!
    Returns a suitable size hint for the section specified by \a logicalIndex.

    \sa sizeHint(), defaultSectionSize(), minimumSectionSize(),
    Qt::SizeHintRole
*/

int QpVertHeaderView::sectionSizeHint(int logicalIndex) const
{
    Q_D(const QpVertHeaderView);

    if (isSectionHidden(logicalIndex))
        return 0;

    if (logicalIndex < 0 || logicalIndex >= count())
        return -1;

    QSize size;

    QVariant value = d->model->headerData(logicalIndex, Qt::Vertical, Qt::SizeHintRole);

    if (value.isValid())
        size = qvariant_cast<QSize>(value);
    else
        size = sectionSizeFromContents(logicalIndex);

    int hint = size.height();


    int res = qMax( minimumSectionSize() , hint);

    if ( debug_size )
        qDebug() << "QpVertHeaderView::sectionSizeHint(logicalIndex:"<<logicalIndex << ") = " << res ;

    return res;
}


int QpVertHeaderView::visualIndexAt_Y( int y ) const
{
    // y - coordinate in viewport

    Q_D(const QpVertHeaderView);

    int yy = y;

    d->executePostedLayout();
    d->executePostedResize();

    yy += d->offset;

    const int sectionCount = d->sectionCount;


    if( debug_paint ) qDebug() << tblName() << "QpVertHeaderView::visualIndexAt_Y(y:"<<y<<") + d->offset:"<<d->offset<<" = yy:" <<yy;

    if (sectionCount < 1)
    {
        if( debug_paint ) qDebug() << tblName() <<" ???? sectionCount < 1 QpVertHeaderView::visualIndexAt_Y(y:" << y<<")=" << -1;
        return qp::UNKNOWN_VALUE;
    }

    int hh =  d->hrzntl.row_height();

    if( hh == qp::UNKNOWN_VALUE)
    {
        return qp::UNKNOWN_VALUE;
    }
    int ll = d->length;
    if ( yy > d->length )
    {
        if( debug_paint ) qDebug() << tblName() <<" ???? yy:"<<yy<<" > d->length:"<< d->length << " sectionCount:"<<sectionCount << " QpVertHeaderView::visualIndexAt_Y(y:" << y<<"[yy:"<<yy<<"])=" << -1 ;

        return qp::UNKNOWN_VALUE;
    }

    int visual = d->headerVisualIndexAt( yy );

    if (visual < 0)
    {
        if( debug_paint ) qDebug() << tblName() <<" ???? visual < 0 QpVertHeaderView::visualIndexAt_Y(y:" << y<<"[yy:"<<yy<<"])=" << visual << " length" << length()<< " d->sectionCount" <<d->sectionCount;

        d->headerVisualIndexAt( yy );
        return qp::UNKNOWN_VALUE;
    }

    if( debug_paint ) qDebug() << tblName() <<"     return_visual:"<<visual<<" sectionCount:" << d->sectionCount << " d->lengthlength" <<d->length << "  d->offset" <<d->offset;

    return visual;
}


int QpVertHeaderView::logicalIndexAt(int position) const
{
    const int visual = visualIndexAt_Y(position);

    if (visual > -1)
        return logicalIndex(visual);

    return -1;
}


int QpVertHeaderView::sectionSize(int logicalIndex) const
{
    Q_D(const QpVertHeaderView);

    if (isSectionHidden(logicalIndex))
        return 0;

    if (logicalIndex < 0 || logicalIndex >= count())
        return 0;

    int visual = visualIndex(logicalIndex);

    if (visual == -1)
        return 0;

    d->executePostedResize();

    int height = d->hrzntl.row_height();

    return height;

    //return d->headerSectionSize(visual);
}


int QpVertHeaderView::sectionPosition(int row) const
{
    Q_D(const QpVertHeaderView);

    int visual = visualIndex(row);

    // in some cases users may change the selections
    // before we have a chance to do the layout

    if (visual == -1)
        return -1;

    d->executePostedResize();

    int y = d->headerSectionPosition_Y( visual );

    return y;
}


int QpVertHeaderView::sectionViewportPosition(int row) const
{
    Q_D(const QpVertHeaderView);

    if (row >= count())
        return -1;

    int position = sectionPosition( row );

    if (position < 0)
        return position; // the section was hidden

    int offsetPosition = position - d->offset;


    return offsetPosition;
}



void QpVertHeaderView::moveSection(int from, int to)
{
    Q_D(QpVertHeaderView);

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

void QpVertHeaderView::resizeSection(int logical, int size)
{
    Q_D(QpVertHeaderView);

    //    if (logical < 0 || logical >= count())
    //        return;

    //    if (isSectionHidden(logical)) {
    //        d->hiddenSectionSize.insert(logical, size);
    //        return;
    //    }

    //    int visual = visualIndex(logical);
    //    if (visual == -1)
    //        return;

    //    int oldSize = d->headerSectionSize(visual);
    //    if (oldSize == size)
    //        return;

    //    d->executePostedLayout();
    //    d->invalidateCachedSizeHint();

    //    if (stretchLastSection() && visual == d->lastVisibleVisualIndex())
    //        d->lastSectionSize = size;

    //    if (size != oldSize)
    //        d->createSectionSpan(visual, visual, size, d->headerSectionResizeMode(visual));

    //    int w = d->viewport->width();
    //    int h = d->viewport->height();
    //    int pos = sectionViewportPosition(logical);
    //    QRect r;
    //    if (d->orientation == Qt::Horizontal)
    //        if (isRightToLeft())
    //            r.setRect(0, 0, pos + size, h);
    //        else
    //            r.setRect(pos, 0, w - pos, h);
    //    else
    //        r.setRect(0, pos, w, h - pos);

    //    if (d->hasAutoResizeSections()) {
    //        d->doDelayedResizeSections();
    //        r = d->viewport->rect();
    //    }
    //    d->viewport->update(r.normalized());
    //    emit sectionResized(logical, oldSize, size);
}


void QpVertHeaderView::resizeSections(QpVertHeaderView::ResizeMode mode)
{
    Q_D(QpVertHeaderView);

    d->resizeSections(mode, true);
}

bool QpVertHeaderView::isSectionHidden(int logicalIndex) const
{
    Q_D(const QpVertHeaderView);
    d->executePostedLayout();

    if (logicalIndex >= d->sectionHidden.count() || logicalIndex < 0 || logicalIndex >= d->sectionCount)
        return false;

    int visual = visualIndex(logicalIndex);
    Q_ASSERT(visual != -1);
    return d->sectionHidden.testBit(visual);
}


int QpVertHeaderView::hiddenSectionCount() const
{
    Q_D(const QpVertHeaderView);
    return d->hiddenSectionSize.count();
}

void QpVertHeaderView::setSectionHidden(int logicalIndex, bool hide)
{
    Q_D(QpVertHeaderView);

    if (logicalIndex < 0 || logicalIndex >= count())
        return;

    d->executePostedLayout();

    int visual = visualIndex(logicalIndex);
    Q_ASSERT(visual != -1);

    if (hide == d->isVisualIndexHidden(visual))
        return;

    if (hide)
    {
        int size = d->headerSectionHeight(visual);

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


int QpVertHeaderView::count() const
{
    Q_D( const QpVertHeaderView );
    //Q_ASSERT(d->sectionCount == d->headerSectionCount());
    // ### this may affect the lazy layout

    //qDebug()<< "1 pVertHeaderView::count() d->section_Count "<< d->section_Count;

    d->executePostedLayout();

    //qDebug()<< "2 pVertHeaderView::count() " <<d->section_Count;

    return d->sectionCount;
}


int QpVertHeaderView::visualIndex(int logicalIndex) const
{
    Q_D(const QpVertHeaderView);

    if (logicalIndex < 0)
        return -1;

    d->executePostedLayout();

    if (d->visualIndices.isEmpty())
    {
        // nothing has been moved, so we have no mapping
        if (logicalIndex < d->sectionCount)
            return logicalIndex;
    }
    else if (logicalIndex < d->visualIndices.count())
    {
        int visual = d->visualIndices.at(logicalIndex);

        Q_ASSERT(visual < d->sectionCount);

        return visual;
    }
    return -1;
}


int QpVertHeaderView::logicalIndex(int visualIndex) const
{
    Q_D(const QpVertHeaderView);

    if (visualIndex < 0 || visualIndex >= d->sectionCount)
        return qp::UNDEFINED_FLD;

    return d->logicalIndex(visualIndex);
}

// ### Qt 5: change to setSectionsMovable()
void QpVertHeaderView::setMovable(bool movable)
{
    Q_D(QpVertHeaderView);
    d->movableSections = movable;
}

// ### Qt 5: change to sectionsMovable()
bool QpVertHeaderView::isMovable() const
{
    Q_D(const QpVertHeaderView);
    return d->movableSections;
}

// ### Qt 5: change to setSectionsClickable()
void QpVertHeaderView::setClickable(bool clickable)
{
    Q_D(QpVertHeaderView);
    d->clickableSections = clickable;
}

// ### Qt 5: change to sectionsClickable()
bool QpVertHeaderView::isClickable() const
{
    Q_D(const QpVertHeaderView);
    return d->clickableSections;
}

void QpVertHeaderView::setHighlightSections(bool highlight)
{
    Q_D(QpVertHeaderView);
    d->highlightSelected = highlight;
}

bool QpVertHeaderView::highlightSections() const
{
    Q_D(const QpVertHeaderView);
    return d->highlightSelected;
}

void QpVertHeaderView::setResizeMode(ResizeMode mode)
{
    Q_D(QpVertHeaderView);
    initializeSections();
    d->stretchSections = (mode == Stretch ? count() : 0);
    d->contentsSections =  (mode == ResizeToContents ? count() : 0);
    d->setGlobalHeaderResizeMode(mode);
    if (d->hasAutoResizeSections())
        d->doDelayedResizeSections(); // section sizes may change as a result of the new mode
}


// ### Qt 5: change to setSectionResizeMode()
void QpVertHeaderView::setResizeMode(int logicalIndex, ResizeMode mode)
{
    Q_D(QpVertHeaderView);
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

    if (d->hasAutoResizeSections() && d->state == QpVertHeaderViewPrivate::NoState)
        d->doDelayedResizeSections(); // section sizes may change as a result of the new mode
}


QpVertHeaderView::ResizeMode QpVertHeaderView::resizeMode(int logicalIndex) const
{
    Q_D(const QpVertHeaderView);
    int visual = visualIndex(logicalIndex);
    if (visual == -1)
        return Fixed; //the default value
    return d->headerSectionResizeMode(visual);
}


int QpVertHeaderView::stretchSectionCount() const
{
    Q_D(const QpVertHeaderView);
    return d->stretchSections;
}


void QpVertHeaderView::setSortIndicatorShown(bool show)
{
    Q_D(QpVertHeaderView);
    if (d->sortIndicatorShown == show)
        return;

    d->sortIndicatorShown = show;

    if (sortIndicatorSection() < 0 || sortIndicatorSection() > count())
        return;

    if (d->headerSectionResizeMode(sortIndicatorSection()) == ResizeToContents)
        resizeSections();

    d->viewport->update();
}

bool QpVertHeaderView::isSortIndicatorShown() const
{
    Q_D(const QpVertHeaderView);
    return d->sortIndicatorShown;
}

void QpVertHeaderView::setSortIndicator(int logicalIndex, Qt::SortOrder order)
{
    Q_D(QpVertHeaderView);

    // This is so that people can set the position of the sort indicator before the fill the model
    int old = d->sortIndicatorSection;
    d->sortIndicatorSection = logicalIndex;
    d->sortIndicatorOrder = order;

    if (logicalIndex >= d->sectionCount)
    {
        emit sortIndicatorChanged(logicalIndex, order);
        return; // nothing to do
    }

    if (old != logicalIndex
            && ((logicalIndex >= 0 && resizeMode(logicalIndex) == ResizeToContents)
                || old >= d->sectionCount || (old >= 0 && resizeMode(old) == ResizeToContents)))
    {
        resizeSections();
        d->viewport->update();
    }
    else
    {
        if (old >= 0 && old != logicalIndex)
            updateSection(old);
        if (logicalIndex >= 0)
            updateSection(logicalIndex);
    }

    emit sortIndicatorChanged(logicalIndex, order);
}

int QpVertHeaderView::sortIndicatorSection() const
{
    Q_D(const QpVertHeaderView);
    return d->sortIndicatorSection;
}


Qt::SortOrder QpVertHeaderView::sortIndicatorOrder() const
{
    Q_D(const QpVertHeaderView);
    return d->sortIndicatorOrder;
}


bool QpVertHeaderView::stretchLastSection() const
{
    Q_D(const QpVertHeaderView);
    return d->stretchLastSection;
}

void QpVertHeaderView::setStretchLastSection(bool stretch)
{
    Q_D(QpVertHeaderView);
    d->stretchLastSection = stretch;

    if (d->state != QpVertHeaderViewPrivate::NoState)
        return;

    if (stretch)
        resizeSections();
    else if (count())
        resizeSection(count() - 1, d->defaultSectionSize);
}


bool QpVertHeaderView::cascadingSectionResizes() const
{
    Q_D(const QpVertHeaderView);
    return d->cascadingResizing;
}

void QpVertHeaderView::setCascadingSectionResizes(bool enable)
{
    Q_D(QpVertHeaderView);
    d->cascadingResizing = enable;
}

int QpVertHeaderView::defaultSectionSize() const
{
    Q_D(const QpVertHeaderView);

    return d->hrzntl.row_height();

    //return d->defaultSectionSize;
}

void QpVertHeaderView::setDefaultSectionSize(int size)
{
    Q_D(QpVertHeaderView);

    d->setDefaultSectionSize( size );
}


int QpVertHeaderView::minimumSectionSize() const
{
    Q_D(const QpVertHeaderView);

    //Q_ASSERT (d->hrzntl != 0);

    int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);

    int min_height = margin *2; //d->minimumSectionSize; //d->hrzntl.row_height();

    return min_height;

}

void QpVertHeaderViewPrivate::setDefaultValues() //!!
{
    const int defH = hrzntl.row_height();

    defaultSectionSize = qMax( minimumSectionSize , default_section_height );

    defaultAlignment = Qt::AlignLeft|Qt::AlignVCenter;

    qDebug() << "QpVertHeaderViewPrivate::setDefaultValues() defaultSectionSize:"<<defaultSectionSize;
}

void QpVertHeaderView::setMinimumSectionSize(int size)
{
    Q_D(QpVertHeaderView);

    if ( debug_size )qDebug() << "QpVertHeaderView::setMinimumSectionSize(size:"<<size<<")";

    d->minimumSectionSize = size;
}


Qt::Alignment QpVertHeaderView::defaultAlignment() const
{
    Q_D(const QpVertHeaderView);
    return d->defaultAlignment;
}

void QpVertHeaderView::setDefaultAlignment(Qt::Alignment alignment)
{
    Q_D(QpVertHeaderView);
    if (d->defaultAlignment == alignment)
        return;

    d->defaultAlignment = alignment;
    d->viewport->update();
}


void QpVertHeaderView::doItemsLayout()
{
    initializeSections();

    QpAbstractItemView::doItemsLayout();

}


bool QpVertHeaderView::sectionsMoved() const
{
    Q_D(const QpVertHeaderView);
    return !d->visualIndices.isEmpty();
}

/*!
    \since 4.1

    Returns true if sections in the header has been hidden; otherwise returns
    false;

    \sa setSectionHidden()
*/
bool QpVertHeaderView::sectionsHidden() const
{
    Q_D(const QpVertHeaderView);
    return !d->hiddenSectionSize.isEmpty();
}

#ifndef QT_NO_DATASTREAM
/*!
    \since 4.3

    Saves the current state of this header view.

    To restore the saved state, pass the return value to restoreState().

    \sa restoreState()
*/
QByteArray QpVertHeaderView::saveState() const
{
    Q_D(const QpVertHeaderView);
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << QpVertHeaderViewPrivate::VersionMarker;
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
bool QpVertHeaderView::restoreState(const QByteArray &state)
{
    Q_D(QpVertHeaderView);
    if (state.isEmpty())
        return false;
    QByteArray data = state;
    QDataStream stream(&data, QIODevice::ReadOnly);
    int marker;
    int ver;
    stream >> marker;
    stream >> ver;
    if (stream.status() != QDataStream::Ok
            || marker != QpVertHeaderViewPrivate::VersionMarker
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
void QpVertHeaderView::reset()
{
    Q_D(QpVertHeaderView);

    if(debug_init) qDebug() << "1 QpVertHeaderView::reset() section_Count " << d->sectionCount << " d->length" << d->length;

    QpAbstractItemView::reset();
    // it would be correct to call clear, but some apps rely
    // on the header keeping the sections, even after calling reset

    if(debug_init) qDebug() << "2 QpVertHeaderView::reset() section_Count " << d->sectionCount << " d->length" << d->length;

    d->clear(); //!!

    if(debug_init) qDebug() << "3 QpVertHeaderView::reset() section_Count " << d->sectionCount << " d->length" << d->length;

    initializeSections();

    if(debug_init) qDebug() << "4 QpVertHeaderView::reset() section_Count " << d->sectionCount << " length"<<length();

}

/*!
    Updates the changed header sections with the given \a orientation, from
    \a logicalFirst to \a logicalLast inclusive.
*/
void QpVertHeaderView::headerDataChanged(Qt::Orientation orientation, int logicalFirst, int logicalLast)
{
    if ( debug_mdl_signals) qDebug() << "QpVertHeaderView::headerDataChanged ( logicalFirst"<< logicalFirst<<" logicalLast" << logicalLast<< ")";

    Q_D(QpVertHeaderView);
    if (Qt::Vertical != orientation)
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
    const int first = d->headerSectionPosition_Y(firstVisualIndex),
            last = d->headerSectionPosition_Y(lastVisualIndex)
            + d->headerSectionHeight(lastVisualIndex);


    d->viewport->update(0, first, d->viewport->width(), last - first);
}

/*!
    \internal
    \since 4.2

    Updates the section specified by the given \a logicalIndex.
*/

void QpVertHeaderView::updateSection(int logicalIndex)
{
    Q_D(QpVertHeaderView);

    int y =  sectionViewportPosition(logicalIndex);
    int height = sectionSize(logicalIndex);

    d->viewport->update(QRect(0, y , d->viewport->width(), height));
}

/*!
    Resizes the sections according to their size hints. Normally, you do not
    have to call this function.
*/

void QpVertHeaderView::resizeSections()
{
    Q_D(QpVertHeaderView);
    if (d->hasAutoResizeSections())
        d->resizeSections(Interactive, false); // no global resize mode
}

void QpVertHeaderView::slot_sectionsTmplChanged()
{

    Q_D(QpVertHeaderView);

    int rowH = d->hrzntl.row_height();

    d->length = rowH * d->sectionCount; // !!!!

    if( debug_init ) qDebug() << "QpVertHeaderView::slot_sectionsTmplChanged() d->length:"<< d->length <<" rowH:" << rowH <<" d->sectionCount:"<<d->sectionCount;

}

void QpVertHeaderView::sectionsInserted( const QModelIndex &parent,
                                         int logicalFirst,
                                         int logicalLast)
{

    Q_D(QpVertHeaderView);

    if( debug_mdl_signals ) qDebug() << tblName() <<" QpVertHeaderView::sectionsInserted( logicalFirst"<<logicalFirst<<", logicalLast"<< logicalLast<<") d->sectionCount"<<d->sectionCount<<" d->length"<<d->length;


    //int insertCount = logicalLast - logicalFirst + 1;

    //    d->sectionCount += insertCount;

    int rowH = d->hrzntl.row_height();

    if( rowH == qp::UNKNOWN_VALUE)
    {
        if( debug_mdl_signals ) qDebug() << tblName() << "QpVertHeaderView::sectionsInserted rowH == qp::UNKNOWN_VALUE";
        return ;
    }

    //int insertLength = rowH * insertCount;


    //d->length += insertLength;

    if (parent != d->root)
        return; // we only handle changes in the top level

    int oldCount = d->sectionCount;

    d->invalidateCachedSizeHint();

    //    // add the new sections
    //    int insertAt = 0;
    //    for (int spanStart = 0; insertAt < d->sectionSpans.count() && spanStart < logicalFirst; ++insertAt)
    //        spanStart += d->sectionSpans.at(insertAt).count;

    int insertCount = logicalLast - logicalFirst + 1;

    d->sectionCount += insertCount;


    if ( 1==1)//d->sectionSpans.isEmpty() || insertAt >= d->sectionSpans.count())
    {
        int insertLength = d->defaultSectionSize * insertCount;
        d->length += insertLength;

        if( insertLength !=0 )
            if( debug_mdl_signals ) qDebug() << tblName() << "CHANGED! d->length  was:" << d->length << " new:" << d->length + insertLength << " d->sectionCount" << d->sectionCount<<" d->length" << d->length;

        //QpVertHeaderViewPrivate::SectionSpan span(insertLength, insertCount, d->globalResizeMode);
        //d->sectionSpans.append(span);
    }
    //    else if ((d->sectionSpans.at(insertAt).sectionSize() == d->defaultSectionSize)
    //               && d->sectionSpans.at(insertAt).resizeMode == d->globalResizeMode)
    //    {
    //        // add the new sections to an existing span
    //        int insertLength = d->sectionSpans.at(insertAt).sectionSize() * insertCount;
    //        d->length += insertLength;
    //        d->sectionSpans[insertAt].size += insertLength;
    //        d->sectionSpans[insertAt].count += insertCount;
    //    }
    else
    {
        // separate them out into their own span
        //        int insertLength = d->defaultSectionSize * insertCount;
        //        d->length += insertLength;
        //        QpVertHeaderViewPrivate::SectionSpan span(insertLength, insertCount, d->globalResizeMode);
        //        d->sectionSpans.insert(insertAt, span);
    }

    // update sorting column
    if (d->sortIndicatorSection >= logicalFirst)
        d->sortIndicatorSection += insertCount;

    // update resize mode section counts
    if (d->globalResizeMode == Stretch)
        d->stretchSections = d->sectionCount;

    else if (d->globalResizeMode == ResizeToContents)
        d->contentsSections = d->sectionCount;

    // clear selection cache
    d->sectionSelected.clear();

    // update mapping
    if (!d->visualIndices.isEmpty() && !d->logicalIndices.isEmpty()) {

        Q_ASSERT(d->visualIndices.count() == d->logicalIndices.count());

        int mappingCount = d->visualIndices.count();

        for (int i = 0; i < mappingCount; ++i)
        {
            if (d->visualIndices.at(i) >= logicalFirst)
                d->visualIndices[i] += insertCount;
            if (d->logicalIndices.at(i) >= logicalFirst)
                d->logicalIndices[i] += insertCount;
        }
        for (int j = logicalFirst; j <= logicalLast; ++j)
        {
            d->visualIndices.insert(j, j);
            d->logicalIndices.insert(j, j);
        }
    }

    // insert sections into sectionsHidden
    if (!d->sectionHidden.isEmpty())
    {
        QBitArray sectionHidden(d->sectionHidden);
        sectionHidden.resize(sectionHidden.count() + insertCount);
        sectionHidden.fill(false, logicalFirst, logicalLast + 1);

        for (int j = logicalLast + 1; j < sectionHidden.count(); ++j)
            //here we simply copy the old sectionHidden
            sectionHidden.setBit(j, d->sectionHidden.testBit(j - insertCount));

        d->sectionHidden = sectionHidden;
    }

    // insert sections into hiddenSectionSize
    QHash<int, int> newHiddenSectionSize; // from logical index to section size

    for (int i = 0; i < logicalFirst; ++i)
        if (isSectionHidden(i))
            newHiddenSectionSize[i] = d->hiddenSectionSize[i];

    for (int j = logicalLast + 1; j < d->sectionCount; ++j)
        if (isSectionHidden(j))
            newHiddenSectionSize[j] = d->hiddenSectionSize[j - insertCount];
    d->hiddenSectionSize = newHiddenSectionSize;

    d->doDelayedResizeSections();

    emit sectionCountChanged(oldCount, count());

    //    // if the new sections were not updated by resizing, we need to update now
    if (!d->hasAutoResizeSections())
        d->viewport->update();
}

/*!
    This slot is called when sections are removed from the \a parent.
    \a logicalFirst and \a logicalLast signify where the sections were removed.

    If only one section is removed, \a logicalFirst and \a logicalLast will
    be the same.
*/

void QpVertHeaderView::sectionsAboutToBeRemoved(const QModelIndex &parent,
                                                int logicalFirst, int logicalLast)
{
    Q_D(QpVertHeaderView);
    if ( debug_mdl_signals) qDebug() << "QpVertHeaderView::sectionsAboutToBeRemoved( logicalFirst"<< logicalFirst<< " logicalLast"<< logicalLast<< ") nothing d->sectionCount" << d->sectionCount;

    Q_UNUSED(parent);
    Q_UNUSED(logicalFirst);
    Q_UNUSED(logicalLast);
}

void QpVertHeaderViewPrivate::updateHiddenSections(int logicalFirst, int logicalLast)
{
    Q_Q(QpVertHeaderView);
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
    if (!sectionHidden.isEmpty())
    {
        const int newsize = qMin(sectionCount - changeCount, sectionHidden.size());

        QBitArray newSectionHidden(newsize);

        for (int j = 0, k = 0; j < sectionHidden.size(); ++j)
        {
            const int logical = logicalIndex(j);

            if (logical < logicalFirst || logical > logicalLast)
            {
                newSectionHidden[k++] = sectionHidden[j];
            }
        }
        sectionHidden = newSectionHidden;
    }
}

void QpVertHeaderViewPrivate::_q_sectionsRemoved(const QModelIndex &parent,
                                                 int logicalFirst, int logicalLast)
{
    Q_Q(QpVertHeaderView);

    if ( debug_mdl_signals) qDebug() << "QpVertHeaderViewPrivate::_q_sectionsRemoved() sectionCount" << sectionCount << " length:" << length;


    if (parent != root)
        return; // we only handle changes in the top level

    if (qMin(logicalFirst, logicalLast) < 0
            || qMax(logicalLast, logicalFirst) >= sectionCount)
        return;

    int oldCount = q->count();
    int changeCount = logicalLast - logicalFirst + 1;

    updateHiddenSections(logicalFirst, logicalLast);


    //    if (visualIndices.isEmpty() && logicalIndices.isEmpty())
    //    {
    //        //Q_ASSERT(headerSectionCount() == sectionCount);
    //        //removeSectionsFromSpans(logicalFirst, logicalLast);
    //    }
    //    else
    //    {
    //        for (int l = logicalLast; l >= logicalFirst; --l)
    //        {
    //            int visual = visualIndices.at(l);
    //            for (int v = 0; v < sectionCount; ++v)
    //            {
    //                if (v >= logicalIndices.count())
    //                    continue; // the section doesn't exist
    //                if (v > visual)
    //                {
    //                    int logical = logicalIndices.at(v);
    //                    --(visualIndices[logical]);
    //                }

    //                if (logicalIndex(v) > l) // no need to move the positions before l
    //                    --(logicalIndices[v]);
    //            }
    //            logicalIndices.remove(visual);
    //            visualIndices.remove(l);
    //            //Q_ASSERT(headerSectionCount() == sectionCount);
    //            //removeSectionsFromSpans(visual, visual);
    //        }
    //        // ### handle sectionSelection, sectionHidden
    //    }

    sectionCount -= changeCount;

    if( q->tblName() == "goods" && changeCount !=0)
        if ( debug_mdl_signals) qDebug() << "CHANGED  sectionCount"<<sectionCount;

    // update sorting column
    if (sortIndicatorSection >= logicalFirst)
    {
        if (sortIndicatorSection <= logicalLast)
            sortIndicatorSection = -1;
        else
            sortIndicatorSection -= changeCount;
    }

    // if we only have the last section (the "end" position) left, the header is empty
    if ( sectionCount <= 0 )
        clear();

    invalidateCachedSizeHint();


    if ( debug_mdl_signals) qDebug() << "QpVertHeaderViewPrivate::_q_sectionsRemoved() emit q->sectionCountChanged("<< oldCount <<","<< q->count()<<")" ;

    emit q->sectionCountChanged(oldCount, q->count());

    viewport->update();
}

void QpVertHeaderViewPrivate::_q_layoutAboutToBeChanged()
{
    //if there is no row/column we can't have mapping for columns
    //because no QModelIndex in the model would be valid
    // ### this is far from being bullet-proof and we would need a real system to
    // ### map columns or rows persistently

    if ( debug_mdl_signals) qDebug() << "QpVertHeaderViewPrivate::_q_layoutAboutToBeChanged()";

    for (int i = 0; i < sectionHidden.count(); ++i)
    {
        if (sectionHidden.testBit(i)) // ### note that we are using column or row 0
            persistentHiddenSections.append( model->index(logicalIndex(i), 0, root));
    }
}

void QpVertHeaderViewPrivate::_q_layoutChanged()
{
    Q_Q(QpVertHeaderView);

    qDebug() << "QpVertHeaderViewPrivate::_q_layoutChanged()";

    viewport->update();

    if (persistentHiddenSections.isEmpty() || modelIsEmpty())
    {
        if (modelSectionCount() != sectionCount)
            q->initializeSections();

        persistentHiddenSections.clear();

        return;
    }

    QBitArray oldSectionHidden = sectionHidden;

    bool sectionCountChanged = false;

    for (int i = 0; i < persistentHiddenSections.count(); ++i)
    {
        QModelIndex index = persistentHiddenSections.at(i);

        if (index.isValid())
        {
            const int logical =  index.row();
            q->setSectionHidden(logical, true);
            oldSectionHidden.setBit(logical, false);
        }
        else if (!sectionCountChanged && (modelSectionCount() != sectionCount))
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



void QpVertHeaderView::initializeSections()
{
    Q_D(QpVertHeaderView);

    slot_sectionsTmplChanged();

    const int viewSectionCnt = d->sectionCount;
    const int mdlSectionCnt = d->modelSectionCount();

    if(debug_init)  qDebug() << "QpVertHeaderView::initializeSections() d->sectionCount " << d->sectionCount<< "  length:" << d->length;

    if ( mdlSectionCnt <= 0 )
    {
        d->clear();

        emit sectionCountChanged(viewSectionCnt, 0);

    }
    else if ( mdlSectionCnt != viewSectionCnt )
    {
        if(debug_init)  qDebug() << "   newCount != oldCount   newCount:" << mdlSectionCnt << " oldCount:"<< viewSectionCnt;

        int start_section =0;

        if( mdlSectionCnt < viewSectionCnt) // it was a switching to other model or select  with filter
        {
            start_section = 0;
        }
        else
        {
            start_section = qBound(0, viewSectionCnt, mdlSectionCnt - 1);
        }

        initializeSections( start_section, mdlSectionCnt - 1);


        //        if (stretchLastSection()) // we've already gotten the size hint
        //            d->lastSectionSize = sectionSize(logicalIndex(d->sectionCount - 1));

        //make sure we update the hidden sections

        if (mdlSectionCnt < viewSectionCnt)
            d->updateHiddenSections(0, mdlSectionCnt-1);

        if(debug_init)qDebug() <<tblName()<<" QpVertHeaderView::initializeSections() verticalScrollBar()->maximum()" << verticalScrollBar()->maximum() << "d->sectionCount:" << d->sectionCount << " length" << length();

    }

}

void QpVertHeaderView::initializeSections(int start, int end)
{
    Q_D(QpVertHeaderView);

    Q_ASSERT(start >= 0);
    Q_ASSERT(end >= 0);

    if( debug_init ) qDebug() << "QpVertHeaderView::initializeSections(start:"<<start<< ", end:"<<end<<") d->sectionCount "<<d->sectionCount << " d->length" <<d->length;

    d->invalidateCachedSizeHint();

    if (end + 1 < d->sectionCount)
    {
        int newCount = end + 1;

        //d->removeSectionsFromSpans(newCount, d->sectionCount);

        //        if (!d->hiddenSectionSize.isEmpty())
        //        {
        //            if (d->sectionCount - newCount > d->hiddenSectionSize.count())
        //            {
        //                for (int i = end + 1; i < d->sectionCount; ++i)
        //                    d->hiddenSectionSize.remove(i);
        //            }
        //            else
        //            {
        //                QHash<int, int>::iterator it = d->hiddenSectionSize.begin();

        //                while (it != d->hiddenSectionSize.end())
        //                {
        //                    if (it.key() > end)
        //                        it = d->hiddenSectionSize.erase(it);
        //                    else
        //                        ++it;
        //                }
        //            }
        //        }
    }

    int oldCount = d->sectionCount;
    d->sectionCount = end + 1;

    if( oldCount != d->sectionCount)
    {
        if( debug_init ) qDebug() << "CHANGED   d->sectionCount was: " << oldCount << "  new:"<<d->sectionCount << " tblName()"<<tblName() << " QpVertHeaderView::initializeSections(..)";
    }


    //    if (!d->logicalIndices.isEmpty())
    //    {
    //        if (oldCount <= d->sectionCount)
    //        {
    //            d->logicalIndices.resize(d->sectionCount);
    //            d->visualIndices.resize(d->sectionCount);

    //            for (int i = oldCount; i < d->sectionCount; ++i)
    //            {
    //                d->logicalIndices[i] = i;
    //                d->visualIndices[i] = i;
    //            }
    //        }
    //        else
    //        {
    //            int j = 0;
    //            for (int i = 0; i < oldCount; ++i)
    //            {
    //                int v = d->logicalIndices.at(i);

    //                if (v < d->sectionCount)
    //                {
    //                    d->logicalIndices[j] = v;
    //                    d->visualIndices[v] = j;
    //                    j++;
    //                }
    //            }
    //            d->logicalIndices.resize(d->sectionCount);
    //            d->visualIndices.resize(d->sectionCount);
    //        }
    //    }

    if (d->globalResizeMode == Stretch)
        d->stretchSections = d->sectionCount;

    else if (d->globalResizeMode == ResizeToContents)
        d->contentsSections = d->sectionCount;

    if (!d->sectionHidden.isEmpty())
        d->sectionHidden.resize(d->sectionCount);

    //    if (d->sectionCount > oldCount)
    //        d->createSectionSpan(start, end, (end - start + 1) * d->defaultSectionSize, d->globalResizeMode);

    //d->length = (end - start + 1) * d->defaultSectionSize;
    int hh = d->hrzntl.row_height();

    int ll = d->length;
    //d->length = (end - start + 1) * hh;

    if( debug_init ) qDebug() << "CHANGED!  length was:"<< ll<<" d->length:" << d->length<< " row_height" << d->hrzntl.row_height() << "  start " << start <<" end " <<end;


    //Q_ASSERT(d->headerLength() == d->length);

    if (d->sectionCount != oldCount)
        emit sectionCountChanged(oldCount,  d->sectionCount);

    d->viewport->update();
}

void QpVertHeaderView::currentChanged(const QModelIndex &current, const QModelIndex &old)
{
    Q_D(QpVertHeaderView);

    if ( current.row() != old.row())
    {
        if (old.isValid() && old.parent() == d->root)
            d->viewport->update(QRect(0, sectionViewportPosition(old.row()),
                                      d->viewport->width(), sectionSize(old.row())));
        if (current.isValid() && current.parent() == d->root)
            d->viewport->update(QRect(0, sectionViewportPosition(current.row()),
                                      d->viewport->width(), sectionSize(current.row())));
    }
}


/*!
  \reimp
*/

bool QpVertHeaderView::event(QEvent *e)
{
    Q_D(QpVertHeaderView);
    switch (e->type())
    {
    case QEvent::HoverEnter:
    {
        QHoverEvent *he = static_cast<QHoverEvent*>(e);
        d->hover = logicalIndexAt(he->pos());
        if (d->hover != -1)
            updateSection(d->hover);
        break; }
    case QEvent::Leave:
    case QEvent::HoverLeave:
    {
        if (d->hover != -1)
            updateSection(d->hover);
        d->hover = -1;
        break; }

    case QEvent::HoverMove:
    {
        QHoverEvent *he = static_cast<QHoverEvent*>(e);
        int oldHover = d->hover;
        d->hover = logicalIndexAt(he->pos());
        if (d->hover != oldHover) {
            if (oldHover != -1)
                updateSection(oldHover);
            if (d->hover != -1)
                updateSection(d->hover);
        }
        break; }
    case QEvent::Timer: {
        QTimerEvent *te = static_cast<QTimerEvent*>(e);
        if (te->timerId() == d->delayedResize.timerId()) {
            d->delayedResize.stop();
            resizeSections();
        }
        break; }
    default:
        break;
    }
    return QpAbstractItemView::event(e);
}

/*!
  \reimp
*/

void QpVertHeaderView::paintEvent(QPaintEvent *e)
{
    Q_D(QpVertHeaderView);

    if (count() == 0)
        return;


    QPainter painter(d->viewport);

    const QPoint scrollDelayOffset_QPoint = d->scrollDelayOffset;

    QRect translatedEventRect = e->rect();

    if ( debug_paint ) qDebug() << "QpVertHeaderView::paintEvent top" << e->rect()<< " d->offset"<< d->offset << " d->scrollDelayOffset"<< d->scrollDelayOffset << " d->sectionCount:"<<d->sectionCount<< " d->length:"<<d->length << " scrollDelayOffset_QPoint:"<<scrollDelayOffset_QPoint;

    translatedEventRect.translate(scrollDelayOffset_QPoint);
    //qDebug() << "translatedEventRect 2 "<<translatedEventRect;


    int row_start = qp::UNKNOWN_VALUE;
    int row_end = qp::UNKNOWN_VALUE;

    row_start = visualIndexAt_Y( translatedEventRect.top() );

    int btm = translatedEventRect.bottom();
    row_end = visualIndexAt_Y( btm );

    row_start = (row_start == qp::UNKNOWN_VALUE ? 0 : row_start);
    row_end = (row_end == qp::UNKNOWN_VALUE ? count() - 1 : row_end);

    row_start = qMin(row_start, row_end);
    row_end = qMax(row_start, row_end);

    if ( debug_paint ) qDebug() << "QpVertHeaderView::paintEvent " << tblName() << " offset"<<scrollDelayOffset_QPoint
                                <<" row_start" << row_start <<" row_end"<< row_end << " e->rect()"<<e->rect()
                               << " d->sectionCount" <<d->sectionCount<< " d->length" <<d->length << " btm:"<<btm;

    d->prepareSectionSelected(); // clear and resize the bit array

    QRect currentSectionRect;

    int logical;

    const int width = d->viewport->width();
    const int height = d->viewport->height();

    for (int row = row_start; row <= row_end; ++row)
    {
        if ( d->isVisualIndexHidden(row) )
            continue;

        painter.save();

        logical = logicalIndex( row );

        //Q_ASSERT( logical >= 0 );

        if( logical < 0 )
        {
            qDebug() << tblName()<<" ???? ERROR QpVertHeaderView::paintEvent row_end:"<< row_end<<" > d->sectionCount:"<<d->sectionCount;

            int row_end2 = visualIndexAt_Y( btm );

            continue;
        }

        currentSectionRect.setRect(0, sectionViewportPosition(logical), width, sectionSize(logical));

        if ( debug_paint ) qDebug() << "    paintSection row: " << row << " logical:"<<logical<< " currentSectionRect:"<<currentSectionRect;

        currentSectionRect.translate(scrollDelayOffset_QPoint);

        QVariant variant = d->model->headerData(logical, Qt::Vertical, Qt::FontRole);

        if (variant.isValid() && variant.canConvert<QFont>())
        {
            QFont sectionFont = qvariant_cast<QFont>(variant);
            painter.setFont(sectionFont);
        }

        paintSection( &painter, currentSectionRect, logical);

        painter.restore();
    }

    QStyleOption opt;

    opt.init(this);

    // Paint the area beyond where there are indexes

    if (currentSectionRect.right() < translatedEventRect.right())
    {
        // paint to the right
        opt.state |= QStyle::State_Horizontal;

        opt.rect = QRect(currentSectionRect.right() + 1, 0,
                         translatedEventRect.right() - currentSectionRect.right(), height);

        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
    }
    else if (currentSectionRect.bottom() < translatedEventRect.bottom())
    {
        // paint the bottom section
        opt.state &= ~QStyle::State_Horizontal;

        opt.rect = QRect(0, currentSectionRect.bottom() + 1,
                         width, height - currentSectionRect.bottom() - 1);

        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, &painter, this);
    }

#if 0
    // ### visualize section spans
    for (int a = 0, i = 0; i < d->sectionSpans.count(); ++i) {
        QColor color((i & 4 ? 255 : 0), (i & 2 ? 255 : 0), (i & 1 ? 255 : 0));
        if (d->orientation == Qt::Horizontal)
            painter.fillRect(a - d->offset, 0, d->sectionSpans.at(i).size, 4, color);
        else
            painter.fillRect(0, a - d->offset, 4, d->sectionSpans.at(i).size, color);
        a += d->sectionSpans.at(i).size;
    }

#endif
}


void QpVertHeaderView::mousePressEvent(QMouseEvent *e)
{
    Q_D(QpVertHeaderView);
    if (d->state != QpVertHeaderViewPrivate::NoState || e->button() != Qt::LeftButton)
        return;

    int pos = e->y();

    int handle = d->sectionHandleAt(pos);
    d->originalSize = -1; // clear the stored original size

    if (handle == -1)
    {
        d->pressed = logicalIndexAt(pos);

        if (d->clickableSections)
            emit sectionPressed(d->pressed);

        if (d->movableSections)
        {
            d->section = d->target = d->pressed;
            if (d->section == -1)
                return;
            d->state = QpVertHeaderViewPrivate::MoveSection;
            d->setupSectionIndicator(d->section, pos);
        }
        else if (d->clickableSections && d->pressed != -1)
        {
            updateSection(d->pressed);
            d->state = QpVertHeaderViewPrivate::SelectSections;
        }
    }
    else if (resizeMode(handle) == Interactive)
    {
        d->originalSize = sectionSize(handle);
        d->state = QpVertHeaderViewPrivate::ResizeSection;
        d->section = handle;
    }

    d->firstPos = pos;
    d->lastPos = pos;

    d->clearCascadingSections();
}

/*!
  \reimp
*/

void QpVertHeaderView::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(QpVertHeaderView);

    int pos =  e->y();

    if (pos < 0)
        return;
    if (e->buttons() == Qt::NoButton)
    {
#if !defined(Q_WS_MAC)
        // Under Cocoa, when the mouse button is released, may include an extra
        // simulated mouse moved event. The state of the buttons when this event
        // is generated is already "no button" and the code below gets executed
        // just before the mouseReleaseEvent and resets the state. This prevents
        // column dragging from working. So this code is disabled under Cocoa.
        d->state = QpVertHeaderViewPrivate::NoState;
        d->pressed = -1;
#endif
    }
    switch (d->state) {
    case QpVertHeaderViewPrivate::ResizeSection: {
        Q_ASSERT(d->originalSize != -1);
        if (d->cascadingResizing) {
            int delta = pos - d->lastPos;
            int visual = visualIndex(d->section);
            d->cascadingResize(visual, d->headerSectionHeight(visual) + delta);
        } else {
            int delta = pos - d->firstPos;
            resizeSection(d->section, qMax(d->originalSize + delta, minimumSectionSize()));
        }
        d->lastPos = pos;
        return;
    }
    case QpVertHeaderViewPrivate::MoveSection: {
        if (qAbs(pos - d->firstPos) >= QApplication::startDragDistance()
                || !d->sectionIndicator->isHidden()) {
            int visual = visualIndexAt_Y(pos);
            if (visual == -1)
                return;
            int posThreshold = d->headerSectionPosition_Y(visual) + d->headerSectionHeight(visual) / 2;
            int moving = visualIndex(d->section);
            if (visual < moving) {
                if (pos < posThreshold)
                    d->target = d->logicalIndex(visual);
                else
                    d->target = d->logicalIndex(visual + 1);
            } else if (visual > moving) {
                if (pos > posThreshold)
                    d->target = d->logicalIndex(visual);
                else
                    d->target = d->logicalIndex(visual - 1);
            } else {
                d->target = d->section;
            }
            d->updateSectionIndicator(d->section, pos);
        }
        return;
    }
    case QpVertHeaderViewPrivate::SelectSections: {
        int logical = logicalIndexAt(pos);
        if (logical == d->pressed)
            return; // nothing to do
        else if (d->pressed != -1)
            updateSection(d->pressed);
        d->pressed = logical;
        if (d->clickableSections && logical != -1) {
            emit sectionEntered(d->pressed);
            updateSection(d->pressed);
        }
        return;
    }
    case QpVertHeaderViewPrivate::NoState: {
#ifndef QT_NO_CURSOR
        int handle = d->sectionHandleAt(pos);
        bool hasCursor = testAttribute(Qt::WA_SetCursor);

        if (handle != -1 && (resizeMode(handle) == Interactive))
        {
            if (!hasCursor)
                setCursor(Qt::SplitVCursor);
        }
        else if (hasCursor)
        {
            unsetCursor();
        }
#endif
        return;
    }
    default:
        break;
    }
}

/*!
  \reimp
*/

void QpVertHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(QpVertHeaderView);

    int pos = e->y();

    switch (d->state)
    {
    case QpVertHeaderViewPrivate::MoveSection:
        if (!d->sectionIndicator->isHidden())
        { // moving
            int from = visualIndex(d->section);
            Q_ASSERT(from != -1);
            int to = visualIndex(d->target);
            Q_ASSERT(to != -1);
            moveSection(from, to);
            d->section = d->target = -1;
            d->updateSectionIndicator(d->section, pos);
            break;
        } // not moving
    case QpVertHeaderViewPrivate::SelectSections:
        if (!d->clickableSections)
        {
            int section = logicalIndexAt(pos);
            updateSection(section);
        }
        // fall through
    case QpVertHeaderViewPrivate::NoState:
        if (d->clickableSections)
        {
            int section = logicalIndexAt(pos);
            if (section != -1 && section == d->pressed)
            {
                d->flipSortIndicator(section);
                emit sectionClicked(section);
            }
            if (d->pressed != -1)
                updateSection(d->pressed);
        }
        break;
    case QpVertHeaderViewPrivate::ResizeSection:
        d->originalSize = -1;
        d->clearCascadingSections();
        break;
    default:
        break;
    }
    d->state = QpVertHeaderViewPrivate::NoState;
    d->pressed = -1;
}

/*!
  \reimp
*/

void QpVertHeaderView::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(QpVertHeaderView);
    int pos =  e->y();

    int handle = d->sectionHandleAt(pos);

    if (handle > -1 && resizeMode(handle) == Interactive)
    {
        emit sectionHandleDoubleClicked(handle);
#ifndef QT_NO_CURSOR
        Qt::CursorShape splitCursor = Qt::SplitVCursor;

        if (cursor().shape() == splitCursor)
        {
            // signal handlers may have changed the section size
            handle = d->sectionHandleAt(pos);
            if (!(handle > -1 && resizeMode(handle) == Interactive))
                setCursor(Qt::ArrowCursor);
        }
#endif
    } else {
        emit sectionDoubleClicked(logicalIndexAt(e->pos()));
    }
}



bool QpVertHeaderView::viewportEvent(QEvent *e)
{
    Q_D(QpVertHeaderView);
    switch (e->type())
    {
#ifndef QT_NO_TOOLTIP
    case QEvent::ToolTip:
    {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        int logical = logicalIndexAt(he->pos());
        if (logical != -1) {
            QVariant variant = d->model->headerData(logical, Qt::Vertical, Qt::ToolTipRole);
            if (variant.isValid()) {
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
                && d->model->headerData(logical, Qt::Vertical, Qt::WhatsThisRole).isValid())
            return true;
        break; }
    case QEvent::WhatsThis:
    {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        int logical = logicalIndexAt(he->pos());
        if (logical != -1) {
            QVariant whatsthis = d->model->headerData(logical, Qt::Vertical,
                                                      Qt::WhatsThisRole);
            if (whatsthis.isValid()) {
                QWhatsThis::showText(he->globalPos(), whatsthis.toString(), this);
                return true;
            }
        }
        break; }
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_STATUSTIP
    case QEvent::StatusTip:
    {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        int logical = logicalIndexAt(he->pos());
        if (logical != -1) {
            QString statustip = d->model->headerData(logical, Qt::Vertical,
                                                     Qt::StatusTipRole).toString();
            if (!statustip.isEmpty())
                setStatusTip(statustip);
        }
        return true; }
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
        d->state = QpVertHeaderViewPrivate::NoState;
        d->pressed = d->section = d->target = -1;
        d->updateSectionIndicator(d->section, -1);
        break; }
    case QEvent::Wheel:
    {
        QAbstractScrollArea *asa = qobject_cast<QAbstractScrollArea *>(parentWidget());
        if (asa)
            return QApplication::sendEvent(asa->viewport(), e);
        break; }
    default:
        break;
    }
    return QpAbstractItemView::viewportEvent(e);
}


int QpVertHeaderView::is_rowSelected( int row ) const
{
    Q_D(const QpVertHeaderView);

    bool selected = d->isSectionSelected(row);


    return selected;
}

void QpVertHeaderView::paintSection(QPainter *painter, const QRect &rect, int row) const
{
    Q_D(const QpVertHeaderView);

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

        if (row == d->hover)
            state |= QStyle::State_MouseOver;

        if (row == d->pressed)
            state |= QStyle::State_Sunken;

        else if (d->highlightSelected)
        {

            if (d->sectionIntersectsSelection(row))
                state |= QStyle::State_On;

            if (d->isSectionSelected(row))
                state |= QStyle::State_Sunken;
        }

    }

    if (isSortIndicatorShown() && sortIndicatorSection() == row)
        opt.sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
                ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;

    // setup the style options structure
    QVariant textAlignment = d->model->headerData(row,
                                                  Qt::Vertical,
                                                  Qt::TextAlignmentRole);
    opt.rect = rect;
    opt.section = row;
    opt.state |= state;

    opt.textAlignment = Qt::Alignment(textAlignment.isValid()
                                      ? Qt::Alignment(textAlignment.toInt())
                                      : d->defaultAlignment);

    opt.iconAlignment = Qt::AlignVCenter;

    opt.text = d->model->headerData(row, Qt::Vertical,
                                    Qt::DisplayRole).toString();

    if (d->textElideMode != Qt::ElideNone)
        opt.text = opt.fontMetrics.elidedText(opt.text, d->textElideMode , rect.width() - 4);

    QVariant variant = d->model->headerData(row, Qt::Vertical,Qt::DecorationRole);


    opt.icon = qvariant_cast<QIcon>(variant);

    if (opt.icon.isNull())
        opt.icon = qvariant_cast<QPixmap>(variant);

    QVariant foregroundBrush = d->model->headerData(row, Qt::Vertical,
                                                    Qt::ForegroundRole);


    if (foregroundBrush.canConvert<QBrush>())
        opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));

    QPointF oldBO = painter->brushOrigin();

    QVariant backgroundBrush = d->model->headerData(row, Qt::Vertical,
                                                    Qt::BackgroundRole);
    if (backgroundBrush.canConvert<QBrush>())
    {
        opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
        opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
        painter->setBrushOrigin(opt.rect.topLeft());

    }

    int visual = visualIndex(row);

    if( debug_paint ) qDebug() << "QpVertHeaderView::paintSection row " << row << opt.text << " visual:"<<visual<<" opt "<<opt;

    // the section position

    Q_ASSERT(visual != -1);

    if (count() == 1)
        opt.position = QStyleOptionHeader::OnlyOneSection;

    else if (visual == 0)
        opt.position = QStyleOptionHeader::Beginning;

    else if (visual == count() - 1)
        opt.position = QStyleOptionHeader::End;

    else
        opt.position = QStyleOptionHeader::Middle;

    opt.orientation = Qt::Vertical;
    // the selected position

    bool previousSelected = d->isSectionSelected(this->logicalIndex( visual - 1));

    bool nextSelected =  d->isSectionSelected(this->logicalIndex( visual + 1));

    if (previousSelected && nextSelected)
        opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;

    else if (previousSelected)
        opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;

    else if (nextSelected)
        opt.selectedPosition = QStyleOptionHeader::NextIsSelected;

    else
        opt.selectedPosition = QStyleOptionHeader::NotAdjacent;

    // draw the section
    style()->drawControl(QStyle::CE_Header, &opt, painter, this);

    painter->setBrushOrigin(oldBO);
}

QSize QpVertHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    Q_D(const QpVertHeaderView);
    Q_ASSERT(logicalIndex >= 0);

    ensurePolished();

    // use SizeHintRole
    QVariant variant = d->model->headerData(logicalIndex, Qt::Vertical, Qt::SizeHintRole);
    if (variant.isValid())
        return qvariant_cast<QSize>(variant);

    // otherwise use the contents
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.section = logicalIndex;
    QVariant var = d->model->headerData(logicalIndex, Qt::Vertical,
                                        Qt::FontRole);
    QFont fnt;
    if (var.isValid() && var.canConvert<QFont>())
        fnt = qvariant_cast<QFont>(var);
    else
        fnt = font();
    fnt.setBold(true);
    opt.fontMetrics = QFontMetrics(fnt);

    opt.text = d->model->headerData(logicalIndex, Qt::Vertical,
                                    Qt::DisplayRole).toString();

    variant = d->model->headerData(logicalIndex, Qt::Vertical, Qt::DecorationRole);

    opt.icon = qvariant_cast<QIcon>(variant);
    if (opt.icon.isNull())
        opt.icon = qvariant_cast<QPixmap>(variant);
    QSize size = style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), this);
    if (isSortIndicatorShown()) {
        int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, this);
        size.rheight() += size.width() + margin;
    }
    return size;
}

/*!
    Returns the horizontal offset of the header. This is 0 for vertical
    headers.

    \sa offset()
*/

int QpVertHeaderView::horizontalOffset() const
{
    Q_D(const QpVertHeaderView);

    return 0;
}

/*!
    Returns the vertical offset of the header. This is 0 for horizontal
    headers.

    \sa offset()
*/

int QpVertHeaderView::verticalOffset() const
{
    Q_D(const QpVertHeaderView);

    return d->offset;

}

/*!
    \reimp
    \internal
*/

void QpVertHeaderView::updateGeometries()
{
    Q_D(QpVertHeaderView);
    d->layoutChildren();
    if (d->hasAutoResizeSections())
        d->doDelayedResizeSections();
}

/*!
    \reimp
    \internal
*/

void QpVertHeaderView::scrollContentsBy(int dx, int dy)
{
    Q_D(QpVertHeaderView);

    if( debug_scroll ) qDebug() << "QpVertHeaderView::scrollContentsBy(dx:"<<dx<<", dy"<<dy<<")";

    d->scrollDirtyRegion(dx, dy);
}

/*!
    \reimp
    \internal
*/
void QpVertHeaderView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_D(QpVertHeaderView);
    d->invalidateCachedSizeHint();

    if (d->hasAutoResizeSections())
    {
        bool resizeRequired = d->globalResizeMode == ResizeToContents;
        int first =  topLeft.row();
        int last = bottomRight.row();

        for (int i = first; i <= last && !resizeRequired; ++i)
            resizeRequired = (resizeMode(i) == ResizeToContents);
        if (resizeRequired)
            d->doDelayedResizeSections();
    }
}

/*!
    \reimp
    \internal

    Empty implementation because the header doesn't show QModelIndex items.
*/
void QpVertHeaderView::rowsInserted(const QModelIndex &, int, int)
{
    // do nothing
}

/*!
    \reimp
    \internal

    Empty implementation because the header doesn't show QModelIndex items.
*/

QRect QpVertHeaderView::visualRect(const QModelIndex &) const
{
    return QRect();
}

/*!
    \reimp
    \internal

    Empty implementation because the header doesn't show QModelIndex items.
*/

void QpVertHeaderView::scrollTo(const QModelIndex &, ScrollHint)
{
    // do nothing - the header only displays sections
}

/*!
    \reimp
    \internal

    Empty implementation because the header doesn't show QModelIndex items.
*/

qp::SECTION QpVertHeaderView::indexAt(const QPoint &) const
{
    return qp::SECTION();
}

/*!
    \reimp
    \internal

    Empty implementation because the header doesn't show QModelIndex items.
*/

bool QpVertHeaderView::isIndexHidden(const QModelIndex &) const
{
    return true; // the header view has no items, just sections
}

/*!
    \reimp
    \internal

    Empty implementation because the header doesn't show QModelIndex items.
*/

QModelIndex QpVertHeaderView::moveCursor(CursorAction, Qt::KeyboardModifiers)
{
    return QModelIndex();
}

/*!
    \reimp

    Selects the items in the given \a rect according to the specified
    \a flags.

    The base class implementation does nothing.
*/

void QpVertHeaderView::setSelection(const QRect&, QItemSelectionModel::SelectionFlags)
{
    // do nothing
    qDebug() << "QpVertHeaderView::setSelection";
}

/*!
    \internal
*/

QRegion QpVertHeaderView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_D(const QpVertHeaderView);
    const int max = d->modelSectionCount();

    // orientation() == Qt::Vertical
    int top = max;
    int bottom = 0;
    int rangeTop, rangeBottom;

    for (int i = 0; i < selection.count(); ++i)
    {
        QItemSelectionRange rect = selection.at(i);

        if( debug_select ) qDebug() << " QpVertHeaderView::visualRegionForSelection rect:"<<rect;

        if (rect.parent().isValid() || !rect.isValid())
            continue; // we only know about toplevel items
        // FIXME an item inside the range may be the leftmost or rightmost

        rangeTop = visualIndex( rect.top() );

        if (rangeTop == -1) // in some cases users may change the selections
            continue;       // before we have a chance to do the layout

        rangeBottom = visualIndex(rect.bottom());

        if (rangeBottom == -1) // in some cases users may change the selections
            continue;          // before we have a chance to do the layout

        if (rangeTop < top)
            top = rangeTop;

        if (rangeBottom > bottom)
            bottom = rangeBottom;
    }

    int logicalTop = logicalIndex( top );
    int logicalBottom = logicalIndex( bottom );

    if( debug_select) qDebug() << "logicalTop:" << logicalTop<< "  logicalBottom:"<<logicalBottom;

    if (logicalTop == -1 || logicalBottom == -1)
        return QRect();

    int topPos = sectionViewportPosition(logicalTop);

    int bottomPos = sectionViewportPosition(logicalBottom) + sectionSize(logicalBottom);

    QRect rect(0, topPos, width(), bottomPos - topPos);

    if( debug_select) qDebug() << "QpVertHeaderView::visualRegionForSelection rect:" << rect;

    return rect;
}


// private implementation

int QpVertHeaderViewPrivate::sectionHandleAt(int position)
{
    Q_Q(QpVertHeaderView);
    int visual = q->visualIndexAt_Y(position);
    if (visual == -1)
        return -1;
    int log = logicalIndex(visual);
    int pos = q->sectionViewportPosition(log);
    int grip = q->style()->pixelMetric(QStyle::PM_HeaderGripMargin, 0, q);

    bool atLeft = position < pos + grip;
    bool atRight = (position > pos + q->sectionSize(log) - grip);
    //    if (reverse())
    //        qSwap(atLeft, atRight);

    if (atLeft)
    {
        //grip at the beginning of the section
        while(visual > -1)
        {
            int logical = q->logicalIndex(--visual);

            if (!q->isSectionHidden(logical))
                return logical;
        }
    }
    else if (atRight)
    {
        //grip at the end of the section
        return log;
    }
    return -1;
}

void QpVertHeaderViewPrivate::setupSectionIndicator(int section, int position)
{
    Q_Q(QpVertHeaderView);
    if (!sectionIndicator) {
        sectionIndicator = new QLabel(viewport);
    }

    int w, h;
    int p = q->sectionViewportPosition(section);

    w = viewport->width();
    h = q->sectionSize(section);

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

void QpVertHeaderViewPrivate::updateSectionIndicator(int section, int position)
{
    if (!sectionIndicator)
        return;

    if (section == -1 || target == -1) {
        sectionIndicator->hide();
        return;
    }

    sectionIndicator->move(0, position - sectionIndicatorOffset);

    sectionIndicator->show();
}

/*!
    Initialize \a option with the values from this QpVertHeaderView. This method is
    useful for subclasses when they need a QStyleOptionHeader, but do not want
    to fill in all the information themselves.

    \sa QStyleOption::initFrom()
*/
void QpVertHeaderView::initStyleOption(QStyleOptionHeader *option) const
{
    Q_D(const QpVertHeaderView);

    option->initFrom(this);
    option->state = QStyle::State_None | QStyle::State_Raised;
    option->orientation = Qt::Vertical;

    if (isEnabled())
        option->state |= QStyle::State_Enabled;
    option->section = 0;
}

bool QpVertHeaderViewPrivate::isSectionSelected(int section) const
{
    int i = section * 2;

    if( section == qp::LABEL_FLD)
        i=0; //!!


    if (i < 0 || i >= sectionSelected.count())
    {
        if( debug_select ) qDebug() << "QpVertHeaderViewPrivate::isSectionSelected("<<section<<") false =" ;
        return false;
    }

    if (sectionSelected.testBit(i)) // if the value was cached
    {
        if ( debug_select )  qDebug() << "QpVertHeaderViewPrivate::isSectionSelected cached section:" << section<< sectionSelected.testBit(i + 1);

        return sectionSelected.testBit(i + 1);
    }

    bool s = false;

    if(selectionModel)
    {
        s =  selectionModel->isRowSelected(section, root) ;

        sectionSelected.setBit(i + 1, s); // selection state
        sectionSelected.setBit(i, true); // cache state

        if ( debug_select )  qDebug()<< "QpVertHeaderViewPrivate::isSectionSelected("<<section<<")="<<s;
        return s;
    }
    else
    {
        if ( debug_select )  qDebug()<< "QpVertHeaderViewPrivate::isSectionSelected("<<section<<")=false";
        return false;
    }

}


int QpVertHeaderViewPrivate::lastVisibleVisualIndex() const
{
    Q_Q(const QpVertHeaderView);

    for (int visual = q->count()-1; visual >= 0; --visual)
    {
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
void QpVertHeaderViewPrivate::resizeSections(QpVertHeaderView::ResizeMode globalMode, bool useGlobalMode)
{
    Q_Q(QpVertHeaderView);
    //stop the timer in case it is delayed
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
    int lengthToStrech =  viewport->height();

    int numberOfStretchedSections = 0;
    QList<int> section_sizes;

    for (int i = 0; i < sectionCount; ++i)
    {
        if (isVisualIndexHidden(i))
            continue;

        QpVertHeaderView::ResizeMode resizeMode;
        if (useGlobalMode && (i != stretchSection))
            resizeMode = globalMode;
        else
            resizeMode = (i == stretchSection ? QpVertHeaderView::Stretch : headerSectionResizeMode(i));

        if (resizeMode == QpVertHeaderView::Stretch) {
            ++numberOfStretchedSections;
            section_sizes.append(headerSectionHeight(i));
            continue;
        }

        // because it isn't stretch, determine its width and remove that from lengthToStrech
        int sectionSize = 0;
        if (resizeMode == QpVertHeaderView::Interactive || resizeMode == QpVertHeaderView::Fixed) {
            sectionSize = headerSectionHeight(i);
        } else { // resizeMode == QpVertHeaderView::ResizeToContents
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
    if (numberOfStretchedSections > 0 && lengthToStrech > 0) { // we have room to stretch in
        int hintLengthForEveryStretchedSection = lengthToStrech / numberOfStretchedSections;
        stretchSectionLength = qMax(hintLengthForEveryStretchedSection, q->minimumSectionSize());
        pixelReminder = lengthToStrech % numberOfStretchedSections;
    }

    int spanStartSection = 0;
    int previousSectionLength = 0;

    QpVertHeaderView::ResizeMode previousSectionResizeMode = QpVertHeaderView::Interactive;

    // resize each section along the total length
    for (int i = 0; i < sectionCount; ++i)
    {
        int oldSectionLength = headerSectionHeight(i);
        int newSectionLength = -1;

        QpVertHeaderView::ResizeMode newSectionResizeMode = headerSectionResizeMode(i);

        if (isVisualIndexHidden(i))
        {
            newSectionLength = 0;
        }
        else
        {
            QpVertHeaderView::ResizeMode resizeMode;
            if (useGlobalMode)
                resizeMode = globalMode;
            else
                resizeMode = (i == stretchSection
                              ? QpVertHeaderView::Stretch
                              : newSectionResizeMode);
            if (resizeMode == QpVertHeaderView::Stretch && stretchSectionLength != -1)
            {
                if (i == lastVisibleSection)
                    newSectionLength = qMax(stretchSectionLength, lastSectionSize);
                else
                    newSectionLength = stretchSectionLength;
                if (pixelReminder > 0) {
                    newSectionLength += 1;
                    --pixelReminder;
                }
                section_sizes.removeFirst();
            } else {
                newSectionLength = section_sizes.front();
                section_sizes.removeFirst();
            }
        }

        //Q_ASSERT(newSectionLength > 0);

        if ((previousSectionResizeMode != newSectionResizeMode
             || previousSectionLength != newSectionLength) && i > 0)
        {
            //            int spanLength = (i - spanStartSection) * previousSectionLength;
            //            createSectionSpan(spanStartSection, i - 1, spanLength, previousSectionResizeMode);
            //            //Q_ASSERT(headerLength() == length);
            //            spanStartSection = i;
        }

        if (newSectionLength != oldSectionLength)
            emit q->sectionResized(logicalIndex(i), oldSectionLength, newSectionLength);

        previousSectionLength = newSectionLength;
        previousSectionResizeMode = newSectionResizeMode;
    }

    //    createSectionSpan( spanStartSection, sectionCount - 1,
    //                      (sectionCount - spanStartSection) * previousSectionLength,
    //                      previousSectionResizeMode);

    //Q_ASSERT(headerLength() == length);
    resizeRecursionBlock = false;
    viewport->update();
}

int QpVertHeaderViewPrivate::modelSectionCount() const
{
    //qDebug() << "Vert  modelSectionCount() " << model->rowCount( root );
    return model->rowCount( root );
}


void QpVertHeaderViewPrivate::clear()
{
    if (state != NoClear)
    {
        length = 0;
        sectionCount = 0;
        visualIndices.clear();
        logicalIndices.clear();
        sectionSelected.clear();
        sectionHidden.clear();
        hiddenSectionSize.clear();
        //sectionSpans.clear();
    }
}

void QpVertHeaderViewPrivate::flipSortIndicator(int section)
{
    Q_Q(QpVertHeaderView);
    Qt::SortOrder sortOrder;

    if (sortIndicatorSection == section)
    {
        sortOrder = (sortIndicatorOrder == Qt::DescendingOrder) ? Qt::AscendingOrder : Qt::DescendingOrder;
    }
    else
    {
        const QVariant value = model->headerData(section, Qt::Vertical, Qt::InitialSortOrderRole);
        if (value.canConvert(QVariant::Int))
            sortOrder = static_cast<Qt::SortOrder>(value.toInt());
        else
            sortOrder = Qt::AscendingOrder;
    }
    q->setSortIndicator(section, sortOrder);
}

void QpVertHeaderViewPrivate::cascadingResize(int visual, int newSize)
{
    Q_Q(QpVertHeaderView);
    //    const int minimumSize = q->minimumSectionSize();
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
    //        if (!sectionResized)
    //        {
    //            for (int i = visual + 1; i < sectionCount; ++i)
    //            {
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

void QpVertHeaderViewPrivate::setDefaultSectionSize(int size)
{
    Q_Q(QpVertHeaderView);
    defaultSectionSize = size;

    //row_heigth = size;
    //    int currentVisualIndex = 0;

    //    for (int i = 0; i < sectionSpans.count(); ++i)
    //    {
    //        QpVertHeaderViewPrivate::SectionSpan &span = sectionSpans[i];
    //        if (span.size > 0) {
    //            //we resize it if it is not hidden (ie size > 0)
    //            const int newSize = span.count * size;
    //            if (newSize != span.size) {
    //                length += newSize - span.size; //the whole length is changed
    //                const int oldSectionSize = span.sectionSize();
    //                span.size = span.count * size;

    //                for (int i = currentVisualIndex; i < currentVisualIndex + span.count; ++i)
    //                {
    //                    emit q->sectionResized(logicalIndex(i), oldSectionSize, size);
    //                }
    //            }
    //        }
    //        currentVisualIndex += span.count;
    //    }
}

//void QpVertHeaderViewPrivate::resizeSectionSpan(int visualIndex, int oldSize, int newSize)
//{
//    Q_Q(QpVertHeaderView);

//    QpVertHeaderView::ResizeMode mode = headerSectionResizeMode(visualIndex);
//    createSectionSpan(visualIndex, visualIndex, newSize, mode);
//    emit q->sectionResized(logicalIndex(visualIndex), oldSize, newSize);
//}

int QpVertHeaderViewPrivate::headerSectionHeight(int visual) const
{

    return hrzntl.row_height();

}

int QpVertHeaderViewPrivate::headerSectionPosition_Y(int visual) const

{

    return hrzntl.row_height() * visual;

}

int QpVertHeaderViewPrivate::headerVisualIndexAt(int yy) const
{
    Q_Q(const QpVertHeaderView);

    // y - coordinate from 0 ( mot from viewport)

    if( yy > length)
        return  qp::UNKNOWN_VALUE;

    int height = hrzntl.row_height();

    if(height == qp::UNKNOWN_VALUE)
        return qp::UNKNOWN_VALUE;

    Q_ASSERT( height != 0 );

    int row = yy / height;// defaultSectionSize;


    return row;
}

void QpVertHeaderViewPrivate::setHeaderSectionResizeMode(int visual, QpVertHeaderView::ResizeMode mode)
{
    if( debug ) qDebug() << "???????  QpVertHeaderViewPrivate::setHeaderSectionResizeMode ";

}

QpVertHeaderView::ResizeMode QpVertHeaderViewPrivate::headerSectionResizeMode(int visual) const
{
    return globalResizeMode;

}

void QpVertHeaderViewPrivate::setGlobalHeaderResizeMode(QpVertHeaderView::ResizeMode mode)
{
    globalResizeMode = mode;

}

int QpVertHeaderViewPrivate::viewSectionSizeHint(int logical) const
{
    if (QpAbstractItemView *view = qobject_cast<QpAbstractItemView*>(parent))
    {

        int hint = view->sizeHintForRow(logical);

        if( debug_size ) qDebug() << " QpVertHeaderViewPrivate::viewSectionSizeHint("<<logical<<")  =" << hint;

        return hint;
    }
    return 0;
}

int QpVertHeaderViewPrivate::adjustedVisualIndex(int visualIndex) const
{
    if (hiddenSectionSize.count() > 0)
    {
        int adjustedVisualIndex = visualIndex;
        int currentVisualIndex = 0;

        //        for (int i = 0; i < sectionSpans.count(); ++i) {
        //            if (sectionSpans.at(i).size == 0)
        //                adjustedVisualIndex += sectionSpans.at(i).count;
        //            else
        //                currentVisualIndex += sectionSpans.at(i).count;
        //            if (currentVisualIndex >= visualIndex)
        //                break;
        //        }
        visualIndex = adjustedVisualIndex;
    }
    return visualIndex;
}

#ifndef QT_NO_DATASTREAM
void QpVertHeaderViewPrivate::write(QDataStream &out) const
{
    //out << int(orientation);
    out << int(sortIndicatorOrder);
    out << sortIndicatorSection;
    out << sortIndicatorShown;

    out << visualIndices;
    out << logicalIndices;

    out << sectionHidden;
    out << hiddenSectionSize;

    out << length;
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

bool QpVertHeaderViewPrivate::read(QDataStream &in)
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

    in >> length;
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
    globalResizeMode = (QpVertHeaderView::ResizeMode)global;

    //in >> sectionSpans;

    return true;
}

QString QpVertHeaderView::tblName() const
{
    QpTableView* tv = qobject_cast<QpTableView*>(parent());

    if( ! tv)
        return QString();

    return tv->tblName();

}


#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE

#endif // QT_NO_ITEMVIEWS

#include "moc_qp_vertheaderview.cpp"
