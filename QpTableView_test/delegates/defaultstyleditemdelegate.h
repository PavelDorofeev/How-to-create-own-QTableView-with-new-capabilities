#ifndef DEFAULTSTYLEDITEMDELEGATE_H
#define DEFAULTSTYLEDITEMDELEGATE_H

#include <QStyledItemDelegate>

class PblSqlRelationalTableModel;

class DefaultStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DefaultStyledItemDelegate( QObject *parent = 0);
    

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    //QString displayText(const QVariant &value, const QLocale &locale) const ;

//    QSize sizeHint(const QStyleOptionViewItem &option,
//                   const QModelIndex &index) const;

signals:
    
public slots:

private:
    
};

#endif // DEFAULTSTYLEDITEMDELEGATE_H
