/****************************************************************************
** Meta object code from reading C++ file 'qp_abstractitemview.h'
**
** Created: Fri 24. Nov 18:37:44 2023
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QpTableView_test/qp/tableview/qp_abstractitemview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qp_abstractitemview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QpAbstractItemView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   14, // methods
      16,  209, // properties
       6,  257, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   20,   19,   19, 0x05,
      47,   20,   19,   19, 0x05,
      68,   20,   19,   19, 0x05,
      95,   20,   19,   19, 0x05,
     118,   20,   19,   19, 0x05,
     139,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
     157,   19,   19,   19, 0x0a,
     165,   20,   19,   19, 0x0a,
     191,   19,   19,   19, 0x0a,
     207,   19,   19,   19, 0x0a,
     219,   20,   19,   19, 0x0a,
     237,   19,   19,   19, 0x0a,
     254,   20,   19,   19, 0x0a,
     283,   19,   19,   19, 0x0a,
     297,   19,   19,   19, 0x0a,
     314,   20,   19,   19, 0x0a,
     354,  334,   19,   19, 0x09,
     408,  391,   19,   19, 0x09,
     442,  391,   19,   19, 0x09,
     504,  484,   19,   19, 0x09,
     569,  552,   19,   19, 0x09,
     609,   19,   19,   19, 0x09,
     628,   19,   19,   19, 0x09,
     653,   19,   19,   19, 0x09,
     679,  672,   19,   19, 0x09,
     708,  672,   19,   19, 0x09,
     745,  739,   19,   19, 0x09,
     780,  739,   19,   19, 0x09,
     829,  817,   19,   19, 0x09,
     893,  886,   19,   19, 0x09,
     914,  886,   19,   19, 0x09,
     943,  940,   19,   19, 0x08,
     991,  940,   19,   19, 0x08,
    1030,  940,   19,   19, 0x08,
    1070,  940,   19,   19, 0x08,
    1107,  940,   19,   19, 0x08,
    1143,   19,   19,   19, 0x08,
    1163,   19,   19,   19, 0x08,
    1182,   19,   19,   19, 0x08,

 // properties: name, type, flags
    1210, 1205, 0x01095103,
    1225, 1221, 0x02095103,
    1255, 1242, 0x0009510b,
    1268, 1205, 0x01095103,
    1285, 1205, 0x01095003,
    1303, 1205, 0x01095103,
    1315, 1205, 0x01095103,
    1350, 1337, 0x0009510b,
    1378, 1363, 0x0009510b,
    1396, 1205, 0x01095103,
    1431, 1417, 0x0009510b,
    1463, 1445, 0x0009510b,
    1487, 1481, 0x15095103,
    1514, 1496, 0x0009510b,
    1539, 1528, 0x0009510b,
    1558, 1528, 0x0009510b,

 // enums: name, flags, count, data
    1417, 0x0,    5,  281,
    1445, 0x0,    3,  291,
    1579, 0x0,    4,  297,
    1242, 0x1,    7,  305,
    1528, 0x0,    2,  319,
    1337, 0x0,    5,  323,

 // enum data: key, value
    1590, uint(QpAbstractItemView::NoSelection),
    1602, uint(QpAbstractItemView::SingleSelection),
    1618, uint(QpAbstractItemView::MultiSelection),
    1633, uint(QpAbstractItemView::ExtendedSelection),
    1651, uint(QpAbstractItemView::ContiguousSelection),
    1671, uint(QpAbstractItemView::SelectItems),
    1683, uint(QpAbstractItemView::SelectRows),
    1694, uint(QpAbstractItemView::SelectColumns),
    1708, uint(QpAbstractItemView::EnsureVisible),
    1722, uint(QpAbstractItemView::PositionAtTop),
    1736, uint(QpAbstractItemView::PositionAtBottom),
    1753, uint(QpAbstractItemView::PositionAtCenter),
    1770, uint(QpAbstractItemView::NoEditTriggers),
    1785, uint(QpAbstractItemView::CurrentChanged),
    1800, uint(QpAbstractItemView::DoubleClicked),
    1814, uint(QpAbstractItemView::SelectedClicked),
    1830, uint(QpAbstractItemView::EditKeyPressed),
    1845, uint(QpAbstractItemView::AnyKeyPressed),
    1859, uint(QpAbstractItemView::AllEditTriggers),
    1875, uint(QpAbstractItemView::ScrollPerItem),
    1889, uint(QpAbstractItemView::ScrollPerPixel),
    1904, uint(QpAbstractItemView::NoDragDrop),
    1915, uint(QpAbstractItemView::DragOnly),
    1924, uint(QpAbstractItemView::DropOnly),
    1933, uint(QpAbstractItemView::DragDrop),
    1942, uint(QpAbstractItemView::InternalMove),

       0        // eod
};

static const char qt_meta_stringdata_QpAbstractItemView[] = {
    "QpAbstractItemView\0\0index\0"
    "pressed(QModelIndex)\0clicked(QModelIndex)\0"
    "doubleClicked(QModelIndex)\0"
    "activated(QModelIndex)\0entered(QModelIndex)\0"
    "viewportEntered()\0reset()\0"
    "setRootIndex(QModelIndex)\0doItemsLayout()\0"
    "selectAll()\0edit(QModelIndex)\0"
    "clearSelection()\0setCurrentIndex(QModelIndex)\0"
    "scrollToTop()\0scrollToBottom()\0"
    "update(QModelIndex)\0topLeft,bottomRight\0"
    "dataChanged(QModelIndex,QModelIndex)\0"
    "parent,start,end\0rowsInserted(QModelIndex,int,int)\0"
    "rowsAboutToBeRemoved(QModelIndex,int,int)\0"
    "selected,deselected\0"
    "selectionChanged(QItemSelection,QItemSelection)\0"
    "current,previous\0"
    "currentChanged(QModelIndex,QModelIndex)\0"
    "updateEditorData()\0updateEditorGeometries()\0"
    "updateGeometries()\0action\0"
    "verticalScrollbarAction(int)\0"
    "horizontalScrollbarAction(int)\0value\0"
    "verticalScrollbarValueChanged(int)\0"
    "horizontalScrollbarValueChanged(int)\0"
    "editor,hint\0"
    "closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)\0"
    "editor\0commitData(QWidget*)\0"
    "editorDestroyed(QObject*)\0,,\0"
    "_q_columnsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_columnsRemoved(QModelIndex,int,int)\0"
    "_q_columnsInserted(QModelIndex,int,int)\0"
    "_q_rowsInserted(QModelIndex,int,int)\0"
    "_q_rowsRemoved(QModelIndex,int,int)\0"
    "_q_modelDestroyed()\0_q_layoutChanged()\0"
    "_q_headerDataChanged()\0bool\0autoScroll\0"
    "int\0autoScrollMargin\0EditTriggers\0"
    "editTriggers\0tabKeyNavigation\0"
    "showDropIndicator\0dragEnabled\0"
    "dragDropOverwriteMode\0DragDropMode\0"
    "dragDropMode\0Qt::DropAction\0"
    "defaultDropAction\0alternatingRowColors\0"
    "SelectionMode\0selectionMode\0"
    "SelectionBehavior\0selectionBehavior\0"
    "QSize\0iconSize\0Qt::TextElideMode\0"
    "textElideMode\0ScrollMode\0verticalScrollMode\0"
    "horizontalScrollMode\0ScrollHint\0"
    "NoSelection\0SingleSelection\0MultiSelection\0"
    "ExtendedSelection\0ContiguousSelection\0"
    "SelectItems\0SelectRows\0SelectColumns\0"
    "EnsureVisible\0PositionAtTop\0"
    "PositionAtBottom\0PositionAtCenter\0"
    "NoEditTriggers\0CurrentChanged\0"
    "DoubleClicked\0SelectedClicked\0"
    "EditKeyPressed\0AnyKeyPressed\0"
    "AllEditTriggers\0ScrollPerItem\0"
    "ScrollPerPixel\0NoDragDrop\0DragOnly\0"
    "DropOnly\0DragDrop\0InternalMove\0"
};

void QpAbstractItemView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QpAbstractItemView *_t = static_cast<QpAbstractItemView *>(_o);
        switch (_id) {
        case 0: _t->pressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: _t->activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: _t->entered((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: _t->viewportEntered(); break;
        case 6: _t->reset(); break;
        case 7: _t->setRootIndex((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: _t->doItemsLayout(); break;
        case 9: _t->selectAll(); break;
        case 10: _t->edit((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 11: _t->clearSelection(); break;
        case 12: _t->setCurrentIndex((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 13: _t->scrollToTop(); break;
        case 14: _t->scrollToBottom(); break;
        case 15: _t->update((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 16: _t->dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 17: _t->rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 18: _t->rowsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 19: _t->selectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 20: _t->currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 21: _t->updateEditorData(); break;
        case 22: _t->updateEditorGeometries(); break;
        case 23: _t->updateGeometries(); break;
        case 24: _t->verticalScrollbarAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->horizontalScrollbarAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: _t->verticalScrollbarValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->horizontalScrollbarValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->closeEditor((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemDelegate::EndEditHint(*)>(_a[2]))); break;
        case 29: _t->commitData((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 30: _t->editorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 31: _t->d_func()->_q_columnsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 32: _t->d_func()->_q_columnsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 33: _t->d_func()->_q_columnsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 34: _t->d_func()->_q_rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 35: _t->d_func()->_q_rowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 36: _t->d_func()->_q_modelDestroyed(); break;
        case 37: _t->d_func()->_q_layoutChanged(); break;
        case 38: _t->d_func()->_q_headerDataChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QpAbstractItemView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QpAbstractItemView::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_QpAbstractItemView,
      qt_meta_data_QpAbstractItemView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QpAbstractItemView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QpAbstractItemView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QpAbstractItemView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QpAbstractItemView))
        return static_cast<void*>(const_cast< QpAbstractItemView*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int QpAbstractItemView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 39)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 39;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = hasAutoScroll(); break;
        case 1: *reinterpret_cast< int*>(_v) = autoScrollMargin(); break;
        case 2: *reinterpret_cast<int*>(_v) = QFlag(editTriggers()); break;
        case 3: *reinterpret_cast< bool*>(_v) = tabKeyNavigation(); break;
        case 4: *reinterpret_cast< bool*>(_v) = showDropIndicator(); break;
        case 5: *reinterpret_cast< bool*>(_v) = dragEnabled(); break;
        case 6: *reinterpret_cast< bool*>(_v) = dragDropOverwriteMode(); break;
        case 7: *reinterpret_cast< DragDropMode*>(_v) = dragDropMode(); break;
        case 8: *reinterpret_cast< Qt::DropAction*>(_v) = defaultDropAction(); break;
        case 9: *reinterpret_cast< bool*>(_v) = alternatingRowColors(); break;
        case 10: *reinterpret_cast< SelectionMode*>(_v) = selectionMode(); break;
        case 11: *reinterpret_cast< SelectionBehavior*>(_v) = selectionBehavior(); break;
        case 12: *reinterpret_cast< QSize*>(_v) = iconSize(); break;
        case 13: *reinterpret_cast< Qt::TextElideMode*>(_v) = textElideMode(); break;
        case 14: *reinterpret_cast< ScrollMode*>(_v) = verticalScrollMode(); break;
        case 15: *reinterpret_cast< ScrollMode*>(_v) = horizontalScrollMode(); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAutoScroll(*reinterpret_cast< bool*>(_v)); break;
        case 1: setAutoScrollMargin(*reinterpret_cast< int*>(_v)); break;
        case 2: setEditTriggers(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 3: setTabKeyNavigation(*reinterpret_cast< bool*>(_v)); break;
        case 4: setDropIndicatorShown(*reinterpret_cast< bool*>(_v)); break;
        case 5: setDragEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 6: setDragDropOverwriteMode(*reinterpret_cast< bool*>(_v)); break;
        case 7: setDragDropMode(*reinterpret_cast< DragDropMode*>(_v)); break;
        case 8: setDefaultDropAction(*reinterpret_cast< Qt::DropAction*>(_v)); break;
        case 9: setAlternatingRowColors(*reinterpret_cast< bool*>(_v)); break;
        case 10: setSelectionMode(*reinterpret_cast< SelectionMode*>(_v)); break;
        case 11: setSelectionBehavior(*reinterpret_cast< SelectionBehavior*>(_v)); break;
        case 12: setIconSize(*reinterpret_cast< QSize*>(_v)); break;
        case 13: setTextElideMode(*reinterpret_cast< Qt::TextElideMode*>(_v)); break;
        case 14: setVerticalScrollMode(*reinterpret_cast< ScrollMode*>(_v)); break;
        case 15: setHorizontalScrollMode(*reinterpret_cast< ScrollMode*>(_v)); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 16;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QpAbstractItemView::pressed(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QpAbstractItemView::clicked(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QpAbstractItemView::doubleClicked(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QpAbstractItemView::activated(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QpAbstractItemView::entered(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QpAbstractItemView::viewportEntered()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
