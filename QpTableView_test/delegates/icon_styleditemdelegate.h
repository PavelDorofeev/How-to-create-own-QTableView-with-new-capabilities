#ifndef ICON_STYLEDITEMDELEGATE_H
#define ICON_STYLEDITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QIcon>
#include <QSvgRenderer>

class Icon_StyledItemDelegate : public QStyledItemDelegate
{
public:
    Icon_StyledItemDelegate(const QString & fileName1,
                            const QString & fileName2,
                            QObject *parent = 0);

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &idx) const;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    //QIcon icon;
    //QImage image;

    QSvgRenderer *svg_renderer;
    QSvgRenderer *svg_renderer_inv;

private:

};

#endif // ICON_STYLEDITEMDELEGATE_H
