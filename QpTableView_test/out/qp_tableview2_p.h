#ifndef QPTABLEVIEW2_P_H
#define QPTABLEVIEW2_P_H

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

#include <QtCore/QList>
#include <QtCore/QLinkedList>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QDebug>
#include "private/qabstractitemview_p.h"
#include "qp/qp_horheaderview.h"

#ifndef QT_NO_TABLEVIEW

QT_BEGIN_NAMESPACE

class QpTableView2Private : public QAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(QpTableView2)
public:

    QpTableView2Private();


    ~QpTableView2Private(); // !!

    //virtual ~QAbstractItemViewPrivate();

    static const bool debug;
    static const bool debug_init;
    static const bool debug_selection;
    mutable QBasicTimer delayed_Repaint; //!!

    void init( );

    void trimHiddenSelections(QItemSelectionRange *range) const;

    inline bool isHidden(int row, int col) const {
        return verticalHeader->isSectionHidden(row)
                || horizontalHeader->isSectionHidden(col);
    }
    inline int visualRow(int logicalRow) const {
        return verticalHeader->visualIndex(logicalRow);
    }
    inline int visualColumn(int logicalCol) const {
        return horizontalHeader->visualIndex(logicalCol);
    }
    inline int logicalRow(int visualRow) const {
        return verticalHeader->logicalIndex(visualRow);
    }
    inline int logicalColumn(int visualCol) const {
        return horizontalHeader->logicalIndex(visualCol);
    }

    inline int accessibleTable2Index(const QModelIndex &index) const {
        return (index.row() + (horizontalHeader ? 1 : 0)) * (index.model()->columnCount() + (verticalHeader ? 1 : 0))
                + index.column() + (verticalHeader ? 1 : 0) + 1;
    }

    void drawCell(QPainter *painter, const QStyleOptionViewItemV4 &option, const QModelIndex &index);
    void drawCellLabel(QPainter *painter, const QStyleOptionViewItemV4 &option, const QString &txt, bool rowSelected, int row, const qp::LABEL_STYLE & st);

    bool showGrid;
    bool showBetweenRowsBorder;
    Qt::PenStyle gridStyle;
    int rowSectionAnchor;
    int columnSectionAnchor;
    int columnResizeTimerID;
    int rowResizeTimerID;
    QList<int> columnsToUpdate;
    QList<int> rowsToUpdate;
    QpHorHeaderView *horizontalHeader;
    QpVertHeaderView *verticalHeader;
    QWidget *cornerWidget;
    bool sortingEnabled;
    bool geometryRecursionBlock;
    QPoint visualCursor;  // (Row,column) cell coordinates to track through span navigation.

    inline bool isRowHidden(int row) const {
        return verticalHeader->isSectionHidden(row);
    }
    inline bool isColumnHidden(int column) const {
        return horizontalHeader->isSectionHidden(column);
    }
    inline bool isCellEnabled(int row, int column) const {
        return isIndexEnabled(model->index(row, column, root));
    }
    inline bool isVisualRowHiddenOrDisabled(int row, int column) const {
        int r = logicalRow(row);
        int c = logicalColumn(column);
        return isRowHidden(r) || !isCellEnabled(r, c);
    }
    inline bool isVisualColumnHiddenOrDisabled(int row, int column) const {
        int r = logicalRow(row);
        int c = logicalColumn(column);
        return isColumnHidden(c) || !isCellEnabled(r, c);
    }

    //QRect visualSpanRect(const QSpanCollection2::Span &span) const;

    void _q_selectRow(int row);
    void _q_selectColumn(int column);

    void selectRow(int row, bool anchor);
    void selectColumn(int column, bool anchor);

public:
//    void _q_rowsRemoved(const QModelIndex &parent, int start, int end);
//    void _q_rowsInserted(const QModelIndex &parent, int start, int end);
//    void _q_columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
//    void _q_columnsRemoved(const QModelIndex &parent, int start, int end);
//    void _q_columnsInserted(const QModelIndex &parent, int start, int end);
//    void _q_modelDestroyed();
//    void _q_layoutChanged();

#ifndef QT_NO_DRAGANDDROP
    virtual bool dropOn(QDropEvent *event, int *row, int *col, QModelIndex *index);
#endif

    virtual void selectAll(QItemSelectionModel::SelectionFlags command);

#ifndef QT_NO_DRAGANDDROP
    virtual QAbstractItemView::DropIndicatorPosition position(const QPoint &pos, const QRect &rect, const QModelIndex &idx) const;
#endif

    virtual QItemViewPaintPairs draggablePaintPairs(const QModelIndexList &indexes, QRect *r) const;
//    // reimplemented in subclasses
//    virtual void adjustViewOptionsForIndex(QStyleOptionViewItemV4*, const QModelIndex&) const {}

//    virtual bool selectionAllowed(const QModelIndex &index) const {
//        // in some views we want to go ahead with selections, even if the index is invalid
//        return isIndexValid(index) && isIndexSelectable(index);
//    }


//    void _q_updateSpanInsertedRows(const QModelIndex &parent, int start, int end);
//    void _q_updateSpanInsertedColumns(const QModelIndex &parent, int start, int end);
//    void _q_updateSpanRemovedRows(const QModelIndex &parent, int start, int end);
//    void _q_updateSpanRemovedColumns(const QModelIndex &parent, int start, int end);

protected:

};

QT_END_NAMESPACE

#endif // QT_NO_TABLEVIEW

#endif // QPTABLEVIEW2_P_H
