/****************************************************************************
** Meta object code from reading C++ file 'bdf2edf.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../cnvs/bdf2edf.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bdf2edf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN16UI_BDF2EDFwindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN16UI_BDF2EDFwindowE = QtMocHelpers::stringData(
    "UI_BDF2EDFwindow",
    "SelectFileButton",
    "",
    "StartConversion",
    "spinbox1_changed",
    "spinbox2_changed",
    "Select_all_signals",
    "Deselect_all_signals",
    "free_edfheader",
    "hpf_checkbox_changed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN16UI_BDF2EDFwindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x08,    1 /* Private */,
       3,    0,   63,    2, 0x08,    2 /* Private */,
       4,    1,   64,    2, 0x08,    3 /* Private */,
       5,    1,   67,    2, 0x08,    5 /* Private */,
       6,    0,   70,    2, 0x08,    7 /* Private */,
       7,    0,   71,    2, 0x08,    8 /* Private */,
       8,    0,   72,    2, 0x08,    9 /* Private */,
       9,    1,   73,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject UI_BDF2EDFwindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN16UI_BDF2EDFwindowE.offsetsAndSizes,
    qt_meta_data_ZN16UI_BDF2EDFwindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN16UI_BDF2EDFwindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UI_BDF2EDFwindow, std::true_type>,
        // method 'SelectFileButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'StartConversion'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'spinbox1_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'spinbox2_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'Select_all_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Deselect_all_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'free_edfheader'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'hpf_checkbox_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void UI_BDF2EDFwindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UI_BDF2EDFwindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->SelectFileButton(); break;
        case 1: _t->StartConversion(); break;
        case 2: _t->spinbox1_changed((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->spinbox2_changed((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->Select_all_signals(); break;
        case 5: _t->Deselect_all_signals(); break;
        case 6: _t->free_edfheader(); break;
        case 7: _t->hpf_checkbox_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *UI_BDF2EDFwindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UI_BDF2EDFwindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN16UI_BDF2EDFwindowE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UI_BDF2EDFwindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
