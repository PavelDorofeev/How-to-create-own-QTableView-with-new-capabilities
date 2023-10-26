/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPTABLEVIEW_P_H
#define QPTABLEVIEW_P_H

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
#include "qp_horheaderview.h"

#ifndef QT_NO_TABLEVIEW

QT_BEGIN_NAMESPACE

/** \internal
*
* This is a list of span with a binary index to look up quickly a span at a certain index.
*
* The index is a map of map.
* spans are mentaly divided into sub spans so that the start of any subspans doesn't overlap
* with any other subspans. There is no real representation of the subspans.
* The key of the first map is the row where the subspan starts, the value of the first map is
* a list (map) of all subspans that starts at the same row.  It is indexed with its row
*/

class QpTableViewPrivate : public QAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(QpTableView)
public:
    QpTableViewPrivate()
        : showGrid(true),
          gridStyle(Qt::SolidLine),
          rowSectionAnchor(-1),
          columnSectionAnchor(-1),
          columnResizeTimerID(0),
          rowResizeTimerID(0),
          horizontalHeader(0),
          verticalHeader(0),
          sortingEnabled(false),
          geometryRecursionBlock(false),
          visualCursor(QPoint())
    {
        wrapItemText = true;
#ifndef QT_NO_DRAGANDDROP
        overwrite = true;
#endif
    }
    static const bool debug;
    static const bool debug_init;
    static const bool debug_selection;
    mutable QBasicTimer delayed_Repaint; //!!

    void init(const Qp_SECTION_TMPL *matrix);

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

    //int sectionSpanEndLogical(const QpHorHeaderView *header, int logical, int span) const;

    //int sectionSpanSize(const QpHorHeaderView *header, int logical, int span) const;

    //bool spanContainsSection(const QpHorHeaderView *header, int logical, int spanLogical, int span) const;

    //    void drawAndClipSpans(const QRegion &area, QPainter *painter,
    //                          const QStyleOptionViewItemV4 &option, QBitArray *drawn,
    //                          int firstVisualRow, int lastVisualRow, int firstVisualColumn, int lastVisualColumn);

    void drawCell(QPainter *painter, const QStyleOptionViewItemV4 &option, const QModelIndex &index);

    bool showGrid;
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

    void _q_updateSpanInsertedRows(const QModelIndex &parent, int start, int end);
    void _q_updateSpanInsertedColumns(const QModelIndex &parent, int start, int end);
    void _q_updateSpanRemovedRows(const QModelIndex &parent, int start, int end);
    void _q_updateSpanRemovedColumns(const QModelIndex &parent, int start, int end);
};

QT_END_NAMESPACE

#endif // QT_NO_TABLEVIEW

#endif // QPTABLEVIEW_P_H
