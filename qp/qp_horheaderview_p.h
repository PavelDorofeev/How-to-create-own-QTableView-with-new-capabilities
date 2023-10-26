#ifndef QP_HORHEADERVIEW_P_H
#define QP_HORHEADERVIEW_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qabstractitemview_p.h"
#include "qp_common.h"

#ifndef QT_NO_ITEMVIEWS

#include "QtCore/qbitarray.h"
#include "QtGui/qapplication.h"
#include "QtGui/qlabel.h"
#include <QPair>
#include <QMessageBox>

QT_BEGIN_NAMESPACE



class QpHorHeaderViewPrivate: public QAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(QpHorHeaderView)

public:
    enum StateVersion { VersionMarker = 0xff };


    QpHorHeaderViewPrivate()
        : state(NoState),
          offset(0),
          sortIndicatorOrder(Qt::DescendingOrder),
          sortIndicatorSection(0),
          sortIndicatorShown(false),
          lastPos_x(-1),
          lastPos_y(-1), //!!
          firstPos_x(-1),
          firstPos_y(-1), //!!
          originalSize_X(-1),
          originalSize_Y(-1),
          section(-1),
          target(-1),
          pressed(-1),
          pressed_orientation(-1),
          hover(-1),
          length_x(0),
          sectionCount(0),
          //visualSectionCnt(0),
          movableSections(false),
          clickableSections(false),
          highlightSelected(false),
          stretchLastSection(false),
          cascadingResizing(false),
          resizeRecursionBlock(false),
          stretchSections(0),
          contentsSections(0),
          minimumSectionSize(-1),
          lastSectionSize(0),
          sectionIndicatorOffset(0),
          sectionIndicator(0),
          globalResizeMode(QpHorHeaderView::Interactive),
          grid(0),
          lines(0)
    {






    }
public:
    bool set_line_height( int line , int height); //!!
    int get_line_height( int line ); //!!

    // -------------------------------- Pbl ---------------------------------------
    static const bool debug;
    static const bool debug_selection;
    static const bool debug_resize ;
    int lines;

    //QList < int > offsets_x; // 100,50,40,90,..
    QList < int > offsets_x; // 0,100,50,40,90,..

    //QList < int > offsets_y; //  30,60,..
    QList < int > offsets_y; //  0,30,60,..

    //QList < int > offsets_y_koef; //  30,60,..

    QList < QList<int> > visual_matrix;   // lines / col
    // line0 : 0 1 2 2 5
    // line1 : 0 1 3 4 5

    //QList < CELL_NODES > visual_section; // left top section and right bottom section

    QHash<int , qp::CELL_NODES > map;

    void setLines(int lines);

    mutable int grid;

    int row_height() const ;

    qp::CELL_NODES get_nodes( int logicalIndex ); //!!

    void recalculateLineHeigth_from( int from, int height);

    // -----------------------------------------------------------------------------

    int lastVisibleVisualIndex() const;
    //int sectionHandleAt(int position); //!!
    int sectionHandleAt(const QPoint &point, Qt::Orientation & ret);//!!
    void setupSectionIndicator(int section, int position);
    void updateSectionIndicator(int section, int position);
    void updateHiddenSections(int logicalFirst, int logicalLast);
    void resizeSections(QpHorHeaderView::ResizeMode globalMode, bool useGlobalMode = false);
    void _q_sectionsRemoved(const QModelIndex &,int,int);
    void _q_layoutAboutToBeChanged();
    void _q_layoutChanged();

    bool isSectionSelected(int section) const;

    inline bool rowIntersectsSelection(int row) const
    {
        return (selectionModel ? selectionModel->rowIntersectsSelection(row, root) : false);
    }

    inline bool columnIntersectsSelection(int column) const
    {
        return (selectionModel ? selectionModel->columnIntersectsSelection(column, root) : false);
    }

    inline bool sectionIntersectsSelection(int logical) const
    {
        return columnIntersectsSelection(logical);
    }

    inline bool isRowSelected(int row) const
    {
        return (selectionModel ? selectionModel->isRowSelected(row, root) : false);
    }

    inline bool isColumnSelected(int column) const
    {
        return (selectionModel ? selectionModel->isColumnSelected(column, root) : false);
    }

    inline void prepareSectionSelected()
    {
        if (!selectionModel || !selectionModel->hasSelection())
            sectionSelected.clear();
        else if (sectionSelected.count() != sectionCount * 2)
            sectionSelected.fill(false, sectionCount * 2);
        else sectionSelected.fill(false);
    }

    inline bool reverse() const {
        return q_func()->isRightToLeft();
    }

    inline int logicalIndex(int visualIndex) const
    {
        return visualIndex;

        //return logicalIndices.isEmpty() ? visualIndex : logicalIndices.at(visualIndex);
    }

    inline int visualIndex(int logicalIndex) const
    {
        return logicalIndex;
        //return visualIndices.isEmpty() ? logicalIndex : visualIndices.at(logicalIndex);
    }

    inline void setDefaultValues(Qt::Orientation o)
    {
        defaultSectionSize = 100;

        defaultAlignment =  Qt::Alignment(Qt::AlignCenter);
    }

    inline bool isVisualIndexHidden(int visual) const
    {
        if( ! map.contains( visual ) || ! map [ visual ].visible )
            return false;

        return true;
        //return !sectionHidden.isEmpty() && sectionHidden.at(visual);
    }

    inline void setVisualIndexHidden(int visual, bool hidden)
    {
        if (!sectionHidden.isEmpty()) sectionHidden.setBit(visual, hidden);
    }

    inline bool hasAutoResizeSections() const
    {
        return stretchSections || stretchLastSection || contentsSections;
    }

    QStyleOptionHeader getStyleOption() const;

    inline void invalidateCachedSizeHint() const
    {
        cachedSizeHint = QSize();
    }

    inline void initializeIndexMapping() const
    {
        if (visualIndices.count() != sectionCount
                || logicalIndices.count() != sectionCount)
        {
            visualIndices.resize(sectionCount);
            logicalIndices.resize(sectionCount);

            for (int s = 0; s < sectionCount; ++s)
            {
                visualIndices[s] = s;
                logicalIndices[s] = s;
            }
        }
    }

    inline void clearCascadingSections()
    {
        firstCascadingSection = sectionCount;
        lastCascadingSection = 0;
        cascadingSectionSize.clear();
    }

    inline void saveCascadingSectionSize(int visual, int size)
    {
        if (!cascadingSectionSize.contains(visual))
        {
            cascadingSectionSize.insert(visual, size);
            firstCascadingSection = qMin(firstCascadingSection, visual);
            lastCascadingSection = qMax(lastCascadingSection, visual);
        }
    }

    inline bool sectionIsCascadable(int visual) const
    {
        return headerSectionResizeMode(visual) == QpHorHeaderView::Interactive;
    }

    inline int modelSectionCount() const
    {
        return  model->columnCount(root);
    }

    inline bool modelIsEmpty() const
    {
        return (model->rowCount(root) == 0 || model->columnCount(root) == 0);
    }

    inline void doDelayedResizeSections()
    {
        if (!delayedResize.isActive())
            delayedResize.start(0, q_func());
    }

    inline void executePostedResize() const
    {
        if (delayedResize.isActive() && state == NoState)
        {
            const_cast<QpHorHeaderView*>(q_func())->resizeSections();
        }
    }

    void clear();
    void flipSortIndicator(int section);
    void cascadingResize(int visual, int newSize);

    enum State { NoState, ResizeSection_X,
                 ResizeSection_Y, //!!
                 MoveSection, SelectSections, NoClear } state;

    int offset; // this is viewport offset , viewport offset from x = 0 coordinate
    // for navigate to last column this offset maybe is not right divisible for row height

    //Qt::Orientation orientation; only Horizontal

    Qt::SortOrder sortIndicatorOrder;
    int sortIndicatorSection;
    bool sortIndicatorShown;

    mutable QVector<int> visualIndices; // visualIndex = visualIndices.at(logicalIndex)
    mutable QVector<int> logicalIndices; // logicalIndex = row or column in the model
    mutable QBitArray sectionSelected; // from logical index to bit
    mutable QBitArray sectionHidden; // from visual index to bit
    mutable QHash<int, int> hiddenSectionSize; // from logical index to section size
    mutable QHash<int, int> cascadingSectionSize; // from visual index to section size
    mutable QSize cachedSizeHint;
    mutable QBasicTimer delayedResize;

    int firstCascadingSection;
    int lastCascadingSection;

    int lastPos_x;
    int lastPos_y; //!!

    int firstPos_x;
    int firstPos_y; //!!

    int originalSize_X; // now for hor and vert orientation , this uses for events
    int originalSize_Y;
    int section; // used for resizing and moving sections

    int line; //!! used for resizing lines

    int target;
    int pressed; // -1 if release mouse button (this is logicalIndex)
    int pressed_orientation; // !!
    int pressed_line; // !!
    int hover;

    int length_x;// length in px of horizontal visual sections

    int sectionCount;

    //int visualSectionCnt; // !!
    int visualColumnCount; // !!

    bool movableSections;
    bool clickableSections;
    bool highlightSelected;
    bool stretchLastSection;
    bool cascadingResizing;
    bool resizeRecursionBlock;
    int stretchSections;
    int contentsSections;
    int defaultSectionSize;
    int minimumSectionSize;
    int lastSectionSize; // $$$
    int sectionIndicatorOffset;
    Qt::Alignment defaultAlignment;
    QLabel *sectionIndicator;
    QpHorHeaderView::ResizeMode globalResizeMode;
    QList<QPersistentModelIndex> persistentHiddenSections;

    // header section spans


    void setDefaultSectionSize(int size);


    int headerSectionSize(int visual) const;
    void setHeaderSectionSize( int logical, int newSize); //!!

    //int headerSectionPosition(int visual) const; //remove
    const QRect headerSectionPosition2( int visual ) const ;
    const QRect headerSectionPosition3( int visual ) const;

    //const int headerSectionNumber( int visual ) const;


    int headerVisualIndexAt2(int x, int line) const;

    // resize mode
    void setHeaderSectionResizeMode(int visual, QpHorHeaderView::ResizeMode mode);
    QpHorHeaderView::ResizeMode headerSectionResizeMode(int visual) const;
    void setGlobalHeaderResizeMode(QpHorHeaderView::ResizeMode mode);

    // other
    int viewSectionSizeHint(int logical) const;
    int adjustedVisualIndex(int visualIndex) const;

#ifndef QT_NO_DATASTREAM
    void write(QDataStream &out) const;
    bool read(QDataStream &in);
#endif

};

QT_END_NAMESPACE

#endif // QT_NO_ITEMVIEWS

#endif // QP_HORHEADERVIEW_P_H
