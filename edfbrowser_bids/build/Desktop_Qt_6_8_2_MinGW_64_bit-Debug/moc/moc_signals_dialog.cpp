/****************************************************************************
** Meta object code from reading C++ file 'signals_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../signals_dialog.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signals_dialog.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16UI_SignalswindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN16UI_SignalswindowE = QtMocHelpers::stringData(
    "UI_Signalswindow",
    "show_signals",
    "",
    "SelectAllButtonClicked",
    "DisplayButtonClicked",
    "DisplayCompButtonClicked",
    "AddButtonClicked",
    "SubtractButtonClicked",
    "RemoveButtonClicked",
    "ColorButtonClicked",
    "SpecialButton*",
    "HelpButtonClicked",
    "selected_signals_changed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN16UI_SignalswindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x08,    1 /* Private */,
       3,    0,   77,    2, 0x08,    3 /* Private */,
       4,    0,   78,    2, 0x08,    4 /* Private */,
       5,    0,   79,    2, 0x08,    5 /* Private */,
       6,    0,   80,    2, 0x08,    6 /* Private */,
       7,    0,   81,    2, 0x08,    7 /* Private */,
       8,    0,   82,    2, 0x08,    8 /* Private */,
       9,    1,   83,    2, 0x08,    9 /* Private */,
      11,    0,   86,    2, 0x08,   11 /* Private */,
      12,    0,   87,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject UI_Signalswindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN16UI_SignalswindowE.offsetsAndSizes,
    qt_meta_data_ZN16UI_SignalswindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN16UI_SignalswindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UI_Signalswindow, std::true_type>,
        // method 'show_signals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'SelectAllButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'DisplayButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'DisplayCompButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'AddButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SubtractButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RemoveButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ColorButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<SpecialButton *, std::false_type>,
        // method 'HelpButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selected_signals_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void UI_Signalswindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UI_Signalswindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->show_signals((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->SelectAllButtonClicked(); break;
        case 2: _t->DisplayButtonClicked(); break;
        case 3: _t->DisplayCompButtonClicked(); break;
        case 4: _t->AddButtonClicked(); break;
        case 5: _t->SubtractButtonClicked(); break;
        case 6: _t->RemoveButtonClicked(); break;
        case 7: _t->ColorButtonClicked((*reinterpret_cast< std::add_pointer_t<SpecialButton*>>(_a[1]))); break;
        case 8: _t->HelpButtonClicked(); break;
        case 9: _t->selected_signals_changed(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< SpecialButton* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *UI_Signalswindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UI_Signalswindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN16UI_SignalswindowE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UI_Signalswindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
