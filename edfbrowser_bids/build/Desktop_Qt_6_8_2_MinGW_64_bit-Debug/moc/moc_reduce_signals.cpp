/****************************************************************************
** Meta object code from reading C++ file 'reduce_signals.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../reduce_signals.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'reduce_signals.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN22UI_ReduceSignalsWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN22UI_ReduceSignalsWindowE = QtMocHelpers::stringData(
    "UI_ReduceSignalsWindow",
    "SelectFileButton",
    "",
    "StartConversion",
    "Select_all_signals",
    "Deselect_all_signals",
    "Set_SRdivider_all_signals",
    "spinBox1changed",
    "spinBox2changed",
    "radioButton1Toggled",
    "radioButton2Toggled",
    "helpbuttonpressed",
    "LoadFromJson"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN22UI_ReduceSignalsWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x08,    1 /* Private */,
       3,    0,   81,    2, 0x08,    2 /* Private */,
       4,    0,   82,    2, 0x08,    3 /* Private */,
       5,    0,   83,    2, 0x08,    4 /* Private */,
       6,    0,   84,    2, 0x08,    5 /* Private */,
       7,    1,   85,    2, 0x08,    6 /* Private */,
       8,    1,   88,    2, 0x08,    8 /* Private */,
       9,    1,   91,    2, 0x08,   10 /* Private */,
      10,    1,   94,    2, 0x08,   12 /* Private */,
      11,    0,   97,    2, 0x08,   14 /* Private */,
      12,    0,   98,    2, 0x08,   15 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject UI_ReduceSignalsWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN22UI_ReduceSignalsWindowE.offsetsAndSizes,
    qt_meta_data_ZN22UI_ReduceSignalsWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN22UI_ReduceSignalsWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UI_ReduceSignalsWindow, std::true_type>,
        // method 'SelectFileButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'StartConversion'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Select_all_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Deselect_all_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Set_SRdivider_all_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'spinBox1changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'spinBox2changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'radioButton1Toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'radioButton2Toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'helpbuttonpressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'LoadFromJson'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void UI_ReduceSignalsWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UI_ReduceSignalsWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->SelectFileButton(); break;
        case 1: _t->StartConversion(); break;
        case 2: _t->Select_all_signals(); break;
        case 3: _t->Deselect_all_signals(); break;
        case 4: _t->Set_SRdivider_all_signals(); break;
        case 5: _t->spinBox1changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->spinBox2changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->radioButton1Toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->radioButton2Toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->helpbuttonpressed(); break;
        case 10: _t->LoadFromJson(); break;
        default: ;
        }
    }
}

const QMetaObject *UI_ReduceSignalsWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UI_ReduceSignalsWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN22UI_ReduceSignalsWindowE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UI_ReduceSignalsWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
