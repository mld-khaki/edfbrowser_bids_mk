/****************************************************************************
** Meta object code from reading C++ file 'z_score_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../z_score_dialog.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'z_score_dialog.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15UI_ZScoreWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN15UI_ZScoreWindowE = QtMocHelpers::stringData(
    "UI_ZScoreWindow",
    "startButtonClicked",
    "",
    "get_annotationsButtonClicked",
    "jumpButtonClicked",
    "ZscoreDialogDestroyed",
    "RadioButtonsClicked",
    "defaultButtonClicked",
    "markersMoved",
    "addTraceButtonClicked",
    "shift_page_right",
    "shift_page_left"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN15UI_ZScoreWindowE[] = {

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
       1,    0,   74,    2, 0x08,    1 /* Private */,
       3,    0,   75,    2, 0x08,    2 /* Private */,
       4,    0,   76,    2, 0x08,    3 /* Private */,
       5,    1,   77,    2, 0x08,    4 /* Private */,
       6,    1,   80,    2, 0x08,    6 /* Private */,
       7,    0,   83,    2, 0x08,    8 /* Private */,
       8,    0,   84,    2, 0x08,    9 /* Private */,
       9,    0,   85,    2, 0x08,   10 /* Private */,
      10,    0,   86,    2, 0x08,   11 /* Private */,
      11,    0,   87,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QObjectStar,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject UI_ZScoreWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN15UI_ZScoreWindowE.offsetsAndSizes,
    qt_meta_data_ZN15UI_ZScoreWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN15UI_ZScoreWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UI_ZScoreWindow, std::true_type>,
        // method 'startButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'get_annotationsButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'jumpButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ZscoreDialogDestroyed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QObject *, std::false_type>,
        // method 'RadioButtonsClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'defaultButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'markersMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addTraceButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shift_page_right'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shift_page_left'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void UI_ZScoreWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UI_ZScoreWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->startButtonClicked(); break;
        case 1: _t->get_annotationsButtonClicked(); break;
        case 2: _t->jumpButtonClicked(); break;
        case 3: _t->ZscoreDialogDestroyed((*reinterpret_cast< std::add_pointer_t<QObject*>>(_a[1]))); break;
        case 4: _t->RadioButtonsClicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->defaultButtonClicked(); break;
        case 6: _t->markersMoved(); break;
        case 7: _t->addTraceButtonClicked(); break;
        case 8: _t->shift_page_right(); break;
        case 9: _t->shift_page_left(); break;
        default: ;
        }
    }
}

const QMetaObject *UI_ZScoreWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UI_ZScoreWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN15UI_ZScoreWindowE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UI_ZScoreWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
