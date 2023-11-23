#ifndef COMBOBOX_DELEGATE_H
#define COMBOBOX_DELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QSqlTableModel>
#include <QItemDelegate>

class ComboBoxDelegate  : public QStyledItemDelegate
{
    Q_OBJECT
public:


    explicit ComboBoxDelegate( const QStringList &lst,
                               QWidget *parent);


    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    //    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    //    QSize sizeHint(const QStyleOptionViewItem &option,
    //                   const QModelIndex &index) const;
    QWidget* parent_;


    //    void paint(QPainter *painter,
    //                                  const QStyleOptionViewItem &option,
    //                                  const QModelIndex &index) const;

    QString displayText(const QVariant &value, const QLocale &locale) const ;


protected:

public slots:

private slots:


private:

    QStringList lst;
    QComboBox *cmb;

};


#endif // COMBOBOX_DELEGATE_H
