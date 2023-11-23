/****************************************************************************
** Meta object code from reading C++ file 'qp_tableview.h'
**
** Created: Thu 23. Nov 19:00:03 2023
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QpTableView_test/qp/tableview/qp_tableview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qp_tableview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QpTableView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       5,  134, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   13,   12,   12, 0x0a,
      39,   32,   12,   12, 0x0a,
      57,   13,   12,   12, 0x0a,
      70,   32,   12,   12, 0x0a,
      86,   13,   12,   12, 0x0a,
      99,   32,   12,   12, 0x0a,
     115,   13,   12,   12, 0x0a,
     140,   12,   12,   12, 0x0a,
     163,   32,   12,   12, 0x0a,
     191,   12,   12,   12, 0x0a,
     217,   32,   12,   12, 0x0a,
     240,  235,   12,   12, 0x0a,
     258,  235,   12,   12, 0x0a,
     306,  288,   12,   12, 0x0a,
     367,  345,   12,   12, 0x09,
     414,  389,   12,   12, 0x09,
     463,  439,   12,   12, 0x09,
     492,  487,   12,   12, 0x09,
     530,  512,   12,   12, 0x09,
     555,  512,   12,   12, 0x09,
     583,   12,   12,   12, 0x09,
     609,  605,   12,   12, 0x09,
     635,   12,   12,   12, 0x08,
     653,   12,   12,   12, 0x08,

 // properties: name, type, flags
     679,  674, 0x01095103,
     701,  688, 0x0009510b,
     711,  674, 0x01095103,
     726,  674, 0x01095103,
     735,  674, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QpTableView[] = {
    "QpTableView\0\0row\0selectRow(int)\0column\0"
    "selectColumn(int)\0hideRow(int)\0"
    "hideColumn(int)\0showRow(int)\0"
    "showColumn(int)\0resizeRowToContents(int)\0"
    "resizeRowsToContents()\0"
    "resizeColumnToContents(int)\0"
    "resizeColumnsToContents()\0sortByColumn(int)\0"
    "show\0setShowGrid(bool)\0"
    "setShowBetweenRowBorder(bool)\0"
    "parent,first,last\0"
    "slot_rowsInserted(QModelIndex,int,int)\0"
    "row,oldIndex,newIndex\0rowMoved(int,int,int)\0"
    "column,oldIndex,newIndex\0"
    "columnMoved(int,int,int)\0"
    "row,oldHeight,newHeight\0rowResized(int,int,int)\0"
    "xNum\0xNumsResized_X(int)\0oldCount,newCount\0"
    "rowCountChanged(int,int)\0"
    "columnCountChanged(int,int)\0"
    "linesInRowResized_Y()\0idx\0"
    "slot_clicked(QModelIndex)\0_q_selectRow(int)\0"
    "_q_selectColumn(int)\0bool\0showGrid\0"
    "Qt::PenStyle\0gridStyle\0sortingEnabled\0"
    "wordWrap\0cornerButtonEnabled\0"
};

void QpTableView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QpTableView *_t = static_cast<QpTableView *>(_o);
        switch (_id) {
        case 0: _t->selectRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->selectColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->hideRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->hideColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->showRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->showColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->resizeRowToContents((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->resizeRowsToContents(); break;
        case 8: _t->resizeColumnToContents((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->resizeColumnsToContents(); break;
        case 10: _t->sortByColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->setShowGrid((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->setShowBetweenRowBorder((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->slot_rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 14: _t->rowMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 15: _t->columnMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 16: _t->rowResized((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 17: _t->xNumsResized_X((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->rowCountChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 19: _t->columnCountChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 20: _t->linesInRowResized_Y(); break;
        case 21: _t->slot_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 22: _t->d_func()->_q_selectRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: _t->d_func()->_q_selectColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QpTableView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QpTableView::staticMetaObject = {
    { &QpAbstractItemView::staticMetaObject, qt_meta_stringdata_QpTableView,
      qt_meta_data_QpTableView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QpTableView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QpTableView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QpTableView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QpTableView))
        return static_cast<void*>(const_cast< QpTableView*>(this));
    return QpAbstractItemView::qt_metacast(_clname);
}

int QpTableView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QpAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = showGrid(); break;
        case 1: *reinterpret_cast< Qt::PenStyle*>(_v) = gridStyle(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isSortingEnabled(); break;
        case 3: *reinterpret_cast< bool*>(_v) = wordWrap(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isCornerButtonEnabled(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setShowGrid(*reinterpret_cast< bool*>(_v)); break;
        case 1: setGridStyle(*reinterpret_cast< Qt::PenStyle*>(_v)); break;
        case 2: setSortingEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 3: setWordWrap(*reinterpret_cast< bool*>(_v)); break;
        case 4: setCornerButtonEnabled(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
