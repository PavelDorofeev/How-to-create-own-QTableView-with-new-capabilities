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
#ifndef QP_TABLEVIEW2_H
#define QP_TABLEVIEW2_H

#include <QtGui/qabstractitemview.h>
#include <QtGui/qp/qp_horheaderview.h>
#include "private/qabstractitemview_p.h"
#include "QtCore/qbasictimer.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_TABLEVIEW

class QpHorHeaderView;
class QpVertHeaderView;
class QpTableView2Private;

class __declspec(dllexport) QpTableView2 : public QAbstractItemView
{
    Q_OBJECT
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid)
    Q_PROPERTY(Qt::PenStyle gridStyle READ gridStyle WRITE setGridStyle)
    Q_PROPERTY(bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)
    Q_PROPERTY(bool wordWrap READ wordWrap WRITE setWordWrap)
    Q_PROPERTY(bool cornerButtonEnabled READ isCornerButtonEnabled WRITE setCornerButtonEnabled)

public:
    explicit QpTableView2( QWidget *parent = 0 , Qp_SECTION_TMPL *matrix=0);
    ~QpTableView2();

    //static const int lines;
    //static const int col3;
    static const int defaultRowHeight;
    static const bool debug;
    static const bool debug_paint;
    static const bool debug_event;
    static const bool debug_init;
    static const bool debug_paint_row_col;
    static const bool debug_paint_region;
    static const bool debug_paint_border;
    static const bool debug_selection;
    static const bool debug_scroll;
    static const int correct_width_minus_1;
    static const int correct_height_minus_1;

    int row_selected;
    void updateGeometry(); //!! debug
    //    void update();
    //    void update(const QRect &rect); //
    void delayed_Repaint(); //!
    //void mouseMoveEvent(QMouseEvent *event);//!!

    virtual void setModel(QAbstractItemModel *model, const Qp_SECTION_TMPL & matrix);

    virtual void setRootIndex(const QModelIndex &index);
    void setSelectionModel(QItemSelectionModel *selectionModel);
    void doItemsLayout();

    QpHorHeaderView *horizontalHeader() const;

    QpVertHeaderView *verticalHeader() const;

    bool init_template( const Qp_SECTION_TMPL &matrix );//!!

    void setHorizontalHeader(QpHorHeaderView *header);

    void setVerticalHeader(QpVertHeaderView *header);

    int rowViewportPosition(int row) const;
    int rowAt(int y) const;

    void setRowHeight(int row, int height);

    void setLineHeightInRow(int line, int height) const; //!

    int rowHeight(int row) const;

    bool is_RowSelected(int row); //!!
    void updateGeometries_();
    //int rowLine(int y) const;

    int columnViewportPosition(int column) const;
    QRect columnViewportPosition2(int logicalIndex ) const; // !!
    int columnAt(int x, int y) const; // !!

    void setColumnWidth(int column, int width);
    int columnWidth(int column) const;

    bool isRowHidden(int row) const;
    void setRowHidden(int row, bool hide);

    bool isColumnHidden(int column) const;
    void setColumnHidden(int column, bool hide);

    void setSortingEnabled(bool enable);
    bool isSortingEnabled() const;

    bool showGrid() const;
    bool showBetweenRowBorder() const;

    Qt::PenStyle gridStyle() const;
    void setGridStyle(Qt::PenStyle style);

    void setRowGridStyle(Qt::PenStyle style);

    void setWordWrap(bool on);
    bool wordWrap() const;

    void setCornerButtonEnabled(bool enable);
    bool isCornerButtonEnabled() const;

    QRect visualRect(const QModelIndex &index) const;

    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);

    QModelIndex indexAt(const QPoint &p) const;


    void sortByColumn(int column, Qt::SortOrder order);

public Q_SLOTS:
    void selectRow(int row);
    void selectColumn(int column);
    void hideRow(int row);
    void hideColumn(int column);
    void showRow(int row);
    void showColumn(int column);
    void resizeRowToContents(int row);
    void resizeRowsToContents();
    void resizeColumnToContents(int column);
    void resizeColumnsToContents();
    void sortByColumn(int column);
    void setShowGrid(bool show);
    void setShowBetweenRowBorder(bool show);

    void slot_rowsInserted(const QModelIndex &parent, int first, int last);

protected Q_SLOTS:
    void rowMoved(int row, int oldIndex, int newIndex);
    void columnMoved(int column, int oldIndex, int newIndex);
    void rowResized(int row, int oldHeight, int newHeight);
    void columnResized(int column, int oldWidth, int newWidth);
    void rowCountChanged(int oldCount, int newCount);
    void columnCountChanged(int oldCount, int newCount);
    void columnResized_Y(); //!!

protected:
    QpTableView2( QpTableView2Private &, Qp_SECTION_TMPL *matrix, QWidget *parent);
    void scrollContentsBy(int dx, int dy);

    QStyleOptionViewItem viewOptions() const;
    void paintEvent(QPaintEvent *e);

    void timerEvent(QTimerEvent *event);

    int horizontalOffset() const;
    int verticalOffset() const;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);

    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;
    QModelIndexList selectedIndexes() const;

    void updateGeometries();

    int sizeHintForRow(int row) const;
    int sizeHintForColumn(int column) const;

    void verticalScrollbarAction(int action);
    void horizontalScrollbarAction(int action);

    bool isIndexHidden(const QModelIndex &index) const;

    void selectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected);
    void currentChanged(const QModelIndex &current,
                        const QModelIndex &previous);

private:
    friend class QAccessibleItemView;
    int visualIndex(const QModelIndex &index) const;

    Q_DECLARE_PRIVATE(QpTableView2)
    Q_DISABLE_COPY(QpTableView2)
    Q_PRIVATE_SLOT(d_func(), void _q_selectRow(int))
    Q_PRIVATE_SLOT(d_func(), void _q_selectColumn(int))
    //    Q_PRIVATE_SLOT(d_func(), void _q_updateSpanInsertedRows(QModelIndex,int,int))
    //    Q_PRIVATE_SLOT(d_func(), void _q_updateSpanInsertedColumns(QModelIndex,int,int))
    //    Q_PRIVATE_SLOT(d_func(), void _q_updateSpanRemovedRows(QModelIndex,int,int))
    //    Q_PRIVATE_SLOT(d_func(), void _q_updateSpanRemovedColumns(QModelIndex,int,int))

    int delayed_Repaint_tmr;

};

#endif // QT_NO_TABLEVIEW

QT_END_NAMESPACE

QT_END_HEADER

#endif // QP_TABLEVIEW2_H
