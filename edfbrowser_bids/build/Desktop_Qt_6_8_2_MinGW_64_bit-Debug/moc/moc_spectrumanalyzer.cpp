/****************************************************************************
** Meta object code from reading C++ file 'spectrumanalyzer.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../spectrumanalyzer.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectrumanalyzer.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN21UI_FreqSpectrumWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN21UI_FreqSpectrumWindowE = QtMocHelpers::stringData(
    "UI_FreqSpectrumWindow",
    "update_curve",
    "",
    "sliderMoved",
    "SpectrumDialogDestroyed",
    "print_to_txt",
    "update_flywheel",
    "thr_finished_func",
    "dftsz_value_changed",
    "windowBox_changed",
    "dftsz_box_changed",
    "overlap_box_changed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN21UI_FreqSpectrumWindowE[] = {

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
       3,    1,   75,    2, 0x08,    2 /* Private */,
       4,    1,   78,    2, 0x08,    4 /* Private */,
       5,    0,   81,    2, 0x08,    6 /* Private */,
       6,    1,   82,    2, 0x08,    7 /* Private */,
       7,    0,   85,    2, 0x08,    9 /* Private */,
       8,    1,   86,    2, 0x08,   10 /* Private */,
       9,    1,   89,    2, 0x08,   12 /* Private */,
      10,    1,   92,    2, 0x08,   14 /* Private */,
      11,    1,   95,    2, 0x08,   16 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QObjectStar,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject UI_FreqSpectrumWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_ZN21UI_FreqSpectrumWindowE.offsetsAndSizes,
    qt_meta_data_ZN21UI_FreqSpectrumWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN21UI_FreqSpectrumWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UI_FreqSpectrumWindow, std::true_type>,
        // method 'update_curve'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sliderMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'SpectrumDialogDestroyed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QObject *, std::false_type>,
        // method 'print_to_txt'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'update_flywheel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'thr_finished_func'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'dftsz_value_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'windowBox_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'dftsz_box_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'overlap_box_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void UI_FreqSpectrumWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<UI_FreqSpectrumWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->update_curve(); break;
        case 1: _t->sliderMoved((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->SpectrumDialogDestroyed((*reinterpret_cast< std::add_pointer_t<QObject*>>(_a[1]))); break;
        case 3: _t->print_to_txt(); break;
        case 4: _t->update_flywheel((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->thr_finished_func(); break;
        case 6: _t->dftsz_value_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->windowBox_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->dftsz_box_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->overlap_box_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *UI_FreqSpectrumWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UI_FreqSpectrumWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN21UI_FreqSpectrumWindowE.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int UI_FreqSpectrumWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
