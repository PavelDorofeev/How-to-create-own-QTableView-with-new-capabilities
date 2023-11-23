/****************************************************************************
** Meta object code from reading C++ file 'qp_vertheaderview.h'
**
** Created: Thu 23. Nov 18:41:00 2023
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QpTableView_test/qp/tableview/qp_vertheaderview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qp_vertheaderview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QpVertHeaderView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       7,  119, // properties
       1,  140, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: signature, parameters, type, tag, flags
      61,   18,   17,   17, 0x05,
     116,   87,   17,   17, 0x05,
     157,  144,   17,   17, 0x05,
     177,  144,   17,   17, 0x05,
     197,  144,   17,   17, 0x05,
     217,  144,   17,   17, 0x05,
     261,  243,   17,   17, 0x05,
     290,  144,   17,   17, 0x05,
     340,  322,   17,   17, 0x05,
     392,   17,   17,   17, 0x05,
     431,  412,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     478,  471,   17,   17, 0x0a,
     505,  493,   17,   17, 0x0a,
     537,   17,   17,   17, 0x0a,
     599,  562,   17,   17, 0x0a,
     642,  144,   17,   17, 0x09,
     661,   17,   17,   17, 0x09,
     710,  678,   17,   17, 0x09,
     748,  678,   17,   17, 0x09,
     794,  678,   17,   17, 0x08,
     834,   17,   17,   17, 0x08,

 // properties: name, type, flags
     867,  862, 0x01095003,
     885,  862, 0x01095103,
     903,  862, 0x01095103,
     922,  862, 0x01095103,
     950,  946, 0x02095103,
     969,  946, 0x02095103,
    1002,  988, 0x0009510b,

 // enums: name, flags, count, data
    1019, 0x0,    5,  144,

 // enum data: key, value
    1030, uint(QpVertHeaderView::Interactive),
    1042, uint(QpVertHeaderView::Stretch),
    1050, uint(QpVertHeaderView::Fixed),
    1056, uint(QpVertHeaderView::ResizeToContents),
    1073, uint(QpVertHeaderView::Custom),

       0        // eod
};

static const char qt_meta_stringdata_QpVertHeaderView[] = {
    "QpVertHeaderView\0\0"
    "logicalIndex,oldVisualIndex,newVisualIndex\0"
    "sectionMoved(int,int,int)\0"
    "logicalIndex,oldSize,newSize\0"
    "sectionResized(int,int,int)\0logicalIndex\0"
    "sectionPressed(int)\0sectionClicked(int)\0"
    "sectionEntered(int)\0sectionDoubleClicked(int)\0"
    "oldCount,newCount\0sectionCountChanged(int,int)\0"
    "sectionHandleDoubleClicked(int)\0"
    "logicalIndex,mode\0"
    "sectionAutoResize(int,QpVertHeaderView::ResizeMode)\0"
    "geometriesChanged()\0logicalIndex,order\0"
    "sortIndicatorChanged(int,Qt::SortOrder)\0"
    "offset\0setOffset(int)\0visualIndex\0"
    "setOffsetToSectionPosition(int)\0"
    "setOffsetToLastSection()\0"
    "orientation,logicalFirst,logicalLast\0"
    "headerDataChanged(Qt::Orientation,int,int)\0"
    "updateSection(int)\0resizeSections()\0"
    "parent,logicalFirst,logicalLast\0"
    "sectionsInserted(QModelIndex,int,int)\0"
    "sectionsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_sectionsRemoved(QModelIndex,int,int)\0"
    "_q_layoutAboutToBeChanged()\0bool\0"
    "showSortIndicator\0highlightSections\0"
    "stretchLastSection\0cascadingSectionResizes\0"
    "int\0defaultSectionSize\0minimumSectionSize\0"
    "Qt::Alignment\0defaultAlignment\0"
    "ResizeMode\0Interactive\0Stretch\0Fixed\0"
    "ResizeToContents\0Custom\0"
};

void QpVertHeaderView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QpVertHeaderView *_t = static_cast<QpVertHeaderView *>(_o);
        switch (_id) {
        case 0: _t->sectionMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->sectionResized((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->sectionPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->sectionClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->sectionEntered((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->sectionDoubleClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->sectionCountChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->sectionHandleDoubleClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->sectionAutoResize((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QpVertHeaderView::ResizeMode(*)>(_a[2]))); break;
        case 9: _t->geometriesChanged(); break;
        case 10: _t->sortIndicatorChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Qt::SortOrder(*)>(_a[2]))); break;
        case 11: _t->setOffset((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->setOffsetToSectionPosition((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->setOffsetToLastSection(); break;
        case 14: _t->headerDataChanged((*reinterpret_cast< Qt::Orientation(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 15: _t->updateSection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->resizeSections(); break;
        case 17: _t->sectionsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 18: _t->sectionsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 19: _t->d_func()->_q_sectionsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 20: _t->d_func()->_q_layoutAboutToBeChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QpVertHeaderView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QpVertHeaderView::staticMetaObject = {
    { &QpAbstractItemView::staticMetaObject, qt_meta_stringdata_QpVertHeaderView,
      qt_meta_data_QpVertHeaderView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QpVertHeaderView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QpVertHeaderView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QpVertHeaderView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QpVertHeaderView))
        return static_cast<void*>(const_cast< QpVertHeaderView*>(this));
    return QpAbstractItemView::qt_metacast(_clname);
}

int QpVertHeaderView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QpAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isSortIndicatorShown(); break;
        case 1: *reinterpret_cast< bool*>(_v) = highlightSections(); break;
        case 2: *reinterpret_cast< bool*>(_v) = stretchLastSection(); break;
        case 3: *reinterpret_cast< bool*>(_v) = cascadingSectionResizes(); break;
        case 4: *reinterpret_cast< int*>(_v) = defaultSectionSize(); break;
        case 5: *reinterpret_cast< int*>(_v) = minimumSectionSize(); break;
        case 6: *reinterpret_cast< Qt::Alignment*>(_v) = defaultAlignment(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSortIndicatorShown(*reinterpret_cast< bool*>(_v)); break;
        case 1: setHighlightSections(*reinterpret_cast< bool*>(_v)); break;
        case 2: setStretchLastSection(*reinterpret_cast< bool*>(_v)); break;
        case 3: setCascadingSectionResizes(*reinterpret_cast< bool*>(_v)); break;
        case 4: setDefaultSectionSize(*reinterpret_cast< int*>(_v)); break;
        case 5: setMinimumSectionSize(*reinterpret_cast< int*>(_v)); break;
        case 6: setDefaultAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QpVertHeaderView::sectionMoved(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QpVertHeaderView::sectionResized(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QpVertHeaderView::sectionPressed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QpVertHeaderView::sectionClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QpVertHeaderView::sectionEntered(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QpVertHeaderView::sectionDoubleClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QpVertHeaderView::sectionCountChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QpVertHeaderView::sectionHandleDoubleClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QpVertHeaderView::sectionAutoResize(int _t1, QpVertHeaderView::ResizeMode _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QpVertHeaderView::geometriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void QpVertHeaderView::sortIndicatorChanged(int _t1, Qt::SortOrder _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_END_MOC_NAMESPACE
