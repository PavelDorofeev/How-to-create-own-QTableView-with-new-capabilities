/****************************************************************************
** Meta object code from reading C++ file 'dialog.h'
**
** Created: Mon 27. Nov 16:28:37 2023
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QpTableView_test/dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Dialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,    8,    7,    7, 0x08,
      37,   34,    7,    7, 0x08,
      65,    7,    7,    7, 0x08,
      89,    7,    7,    7, 0x08,
     122,    7,    7,    7, 0x08,
     153,    7,    7,    7, 0x08,
     186,    7,    7,    7, 0x08,
     220,    7,    7,    7, 0x08,
     267,  259,    7,    7, 0x08,
     296,    7,    7,    7, 0x08,
     327,  321,    7,    7, 0x08,
     367,  321,    7,    7, 0x08,
     403,  259,    7,    7, 0x08,
     442,  321,    7,    7, 0x08,
     488,  321,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Dialog[] = {
    "Dialog\0\0idx\0slot_aaa(QModelIndex)\0pp\0"
    "slot_settinggs_edit(QPoint)\0"
    "on_btn_Update_clicked()\0"
    "on_btn_init_sections_2_clicked()\0"
    "on_btn_init_sections_clicked()\0"
    "on_btn_init_sections_3_clicked()\0"
    "on_btn_sqltableModel_On_clicked()\0"
    "on_btn_QStandardItemModel_On_clicked()\0"
    "checked\0on_chk_grid_on_clicked(bool)\0"
    "on_btn_logFile_clicked()\0index\0"
    "on_cmb_SelectionBehavoir_activated(int)\0"
    "on_cmb_SelectionMode_activated(int)\0"
    "on_chk_betweenRowsBorder_clicked(bool)\0"
    "on_cmb_SelectionMode_currentIndexChanged(int)\0"
    "on_cmb_SelectionBehavoir_currentIndexChanged(int)\0"
};

void Dialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Dialog *_t = static_cast<Dialog *>(_o);
        switch (_id) {
        case 0: _t->slot_aaa((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->slot_settinggs_edit((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->on_btn_Update_clicked(); break;
        case 3: _t->on_btn_init_sections_2_clicked(); break;
        case 4: _t->on_btn_init_sections_clicked(); break;
        case 5: _t->on_btn_init_sections_3_clicked(); break;
        case 6: _t->on_btn_sqltableModel_On_clicked(); break;
        case 7: _t->on_btn_QStandardItemModel_On_clicked(); break;
        case 8: _t->on_chk_grid_on_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->on_btn_logFile_clicked(); break;
        case 10: _t->on_cmb_SelectionBehavoir_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->on_cmb_SelectionMode_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->on_chk_betweenRowsBorder_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->on_cmb_SelectionMode_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->on_cmb_SelectionBehavoir_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Dialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Dialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Dialog,
      qt_meta_data_Dialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Dialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Dialog))
        return static_cast<void*>(const_cast< Dialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
