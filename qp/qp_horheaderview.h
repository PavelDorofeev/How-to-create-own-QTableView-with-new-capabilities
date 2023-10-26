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

#ifndef QP_HORHEADERVIEW_H
#define QP_HORHEADERVIEW_H

#include <QtGui/qabstractitemview.h>

#include "qp_common.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_ITEMVIEWS

class QpHorHeaderViewPrivate;
class QStyleOptionHeader;

typedef QList < QList<int> >  Qp_SECTION_TMPL;

class Q_GUI_EXPORT QpHorHeaderView : public QAbstractItemView
{
    Q_OBJECT
    Q_PROPERTY(bool showSortIndicator READ isSortIndicatorShown WRITE setSortIndicatorShown)
    Q_PROPERTY(bool highlightSections READ highlightSections WRITE setHighlightSections)
    Q_PROPERTY(bool stretchLastSection READ stretchLastSection WRITE setStretchLastSection)
    Q_PROPERTY(bool cascadingSectionResizes READ cascadingSectionResizes WRITE setCascadingSectionResizes)
    Q_PROPERTY(int defaultSectionSize READ defaultSectionSize WRITE setDefaultSectionSize)
    Q_PROPERTY(int minimumSectionSize READ minimumSectionSize WRITE setMinimumSectionSize)
    Q_PROPERTY(Qt::Alignment defaultAlignment READ defaultAlignment WRITE setDefaultAlignment)
    Q_ENUMS(ResizeMode)

public:

    enum ResizeMode
    {
        Interactive,
        Stretch,
        Fixed,
        ResizeToContents,
        Custom = Fixed
    };

    explicit QpHorHeaderView(Qt::Orientation orientation, QWidget *parent = 0);
    virtual ~QpHorHeaderView();

    //--------------------------------------------------------

    static const bool debug ;
    static const bool debug_paint ;
    static const bool debug_init ;
    static const bool debug_selection ;
    static const bool debug_scroll ;
    static const bool debug_resize ;

    static const int default_section_width ;

    //QHash < int , int > cell_floors; // !!! int - col , int - num of col into block

    //--------------------------------------------------------

    virtual void setModel(QAbstractItemModel *model , const Qp_SECTION_TMPL *matrix); //!!

    int row_height() const; // !!
    int line_height( int line ); //!!

    int lastLogicalNum( ) const;
    int leftTopVisualNumX( ) const;
    int rightBtmLogicalNum( ) const;

    Qt::Orientation orientation() const;
    int offset() const;
    int length() const;
    QSize sizeHint() const;
    int sectionSizeHint(int logicalIndex) const;

    int visualIndexAt(int x,  int y) const;
    int visual_xNum_At(int x ) const; // !!
    int xNum_count( ) const; // !!
    int visualIndexAt_end(int x) const;

    bool init_sections_template( const Qp_SECTION_TMPL & matrix );

    //int logicalIndexAt(int position) const;

//    inline int logicalIndexAt(int x, int y) const;
//    inline int logicalIndexAt(const QPoint &pos) const;

    virtual int sectionSize(int logicalIndex) const;

    //virtual int column_size(int col) const; //!!
    virtual int xNum_size(int col) const; //!!

    int get_section_line( int y ) const ;//!!
    int get_section_num( int x ) const ;//!!

    const int lines_count() const;


    //virtual int sectionPosition(int logicalIndex) const;
    virtual const QRect sectionPosition2(int logicalIndex) const;

    const qp::CELL_NODES get_logicalIdex_nodes(int logicalIndex) const;

    //virtual int sectionViewportPosition(int logicalIndex) const;
    virtual QRect sectionViewportPosition2(int logicalIndex) const;


    void setGridWidth( int x ); //!!
    int gridWidth(  ) const; //!!

    //QPoint lastCursor;

    void moveSection(int from, int to);
    void swapSections(int first, int second);

    void resizeSection(int logicalIndex, int size);

    void resizeSection_Y(int logical, int newSize);//!!

    void resizeSections(QpHorHeaderView::ResizeMode mode);

    bool isSectionHidden(int logicalIndex) const;
    void setSectionHidden(int logicalIndex, bool hide);
    int hiddenSectionCount() const;

    inline void hideSection(int logicalIndex);
    inline void showSection(int logicalIndex);

    int count() const;
    int count_of_section() const; //!!
    int left_common_border_x( int logicalIndex ) const; //!!

    int visualIndex(int logicalIndex) const;
    int logicalIndex(int visualIndex) const;
    int logicalIndex_atNum_x_line(int num_x, int num_y) const;//!!

    void setMovable(bool movable);
    bool isMovable() const;

    void setClickable(bool clickable);
    bool isClickable() const;

    void setHighlightSections(bool highlight);
    bool highlightSections() const;

    void setResizeMode(ResizeMode mode);
    void setResizeMode(int logicalIndex, ResizeMode mode);
    ResizeMode resizeMode(int logicalIndex) const;
    int stretchSectionCount() const;

    void setSortIndicatorShown(bool show);
    bool isSortIndicatorShown() const;

    void setSortIndicator(int logicalIndex, Qt::SortOrder order);
    int sortIndicatorSection() const;
    Qt::SortOrder sortIndicatorOrder() const;

    bool stretchLastSection() const;
    void setStretchLastSection(bool stretch);

    bool cascadingSectionResizes() const;
    void setCascadingSectionResizes(bool enable);

    int defaultSectionSize() const;
    void setDefaultSectionSize(int size);

    int minimumSectionSize() const;
    void setMinimumSectionSize(int size);

    Qt::Alignment defaultAlignment() const;
    void setDefaultAlignment(Qt::Alignment alignment);

    void doItemsLayout();
    bool sectionsMoved() const;
    bool sectionsHidden() const;

    int logicalIndexAt(int ax, int line) const;     // !!
    int logicalIndexAt(const QPoint &apos) const;   // !!
    void setSectionSize( int logical, int newSize); //!!

    int minimumLineHeight(); // !!

#ifndef QT_NO_DATASTREAM
    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);
#endif

    void reset();

public Q_SLOTS:
    void setOffset(int offset);
    void setOffsetToSectionPosition(int visualIndex);
    void setOffsetToLastSection();
    void headerDataChanged(Qt::Orientation orientation, int logicalFirst, int logicalLast);

Q_SIGNALS:
    void sectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void sectionResized(int logicalIndex, int oldSize, int newSize);
    void sectionPressed(int logicalIndex);
    void sectionClicked(int logicalIndex);
    void sectionEntered(int logicalIndex);
    void sectionDoubleClicked(int logicalIndex);
    void sectionCountChanged(int oldCount, int newCount);
    void sectionHandleDoubleClicked(int logicalIndex);
    void sectionAutoResize(int logicalIndex, QpHorHeaderView::ResizeMode mode);
    void geometriesChanged();
    void sortIndicatorChanged(int logicalIndex, Qt::SortOrder order);
    void sectionResized_Y();

protected Q_SLOTS:
    void updateSection(int logicalIndex);
    void resizeSections();
    void sectionsInserted(const QModelIndex &parent, int logicalFirst, int logicalLast);
    void sectionsAboutToBeRemoved(const QModelIndex &parent, int logicalFirst, int logicalLast);

protected:
    QpHorHeaderView(QpHorHeaderViewPrivate &dd, Qt::Orientation orientation, QWidget *parent = 0);
    void initialize();

    void initializeSections();
    void initializeSections(int start, int end);
    void currentChanged(const QModelIndex &current, const QModelIndex &old);

    bool event(QEvent *e);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    bool viewportEvent(QEvent *e);

    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    virtual QSize sectionSizeFromContents(int logicalIndex) const;

    int horizontalOffset() const;
    int verticalOffset() const; // only horizontal
    void updateGeometries();
    void scrollContentsBy(int dx, int dy);



    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int start, int end);

    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint);

    QModelIndex indexAt(const QPoint &p) const;
    bool isIndexHidden(const QModelIndex &index) const;

    QModelIndex moveCursor(CursorAction, Qt::KeyboardModifiers);
    void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;
    void initStyleOption(QStyleOptionHeader *option) const;

private:
    Q_PRIVATE_SLOT(d_func(), void _q_sectionsRemoved(const QModelIndex &parent, int logicalFirst, int logicalLast))
    Q_PRIVATE_SLOT(d_func(), void _q_layoutAboutToBeChanged())
    Q_DECLARE_PRIVATE(QpHorHeaderView)
    Q_DISABLE_COPY(QpHorHeaderView)
};


inline void QpHorHeaderView::hideSection(int alogicalIndex)
{
    setSectionHidden(alogicalIndex, true);
}

inline void QpHorHeaderView::showSection(int alogicalIndex)
{
    setSectionHidden(alogicalIndex, false);
}


#endif // QT_NO_ITEMVIEWS

QT_END_NAMESPACE

QT_END_HEADER

#endif // QP_HORHEADERVIEW_H
