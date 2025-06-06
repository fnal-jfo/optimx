/****************************************************************************
** Meta object code from reading C++ file 'OptimPlot.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/OptimPlot.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptimPlot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.3. It"
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
struct qt_meta_tag_ZN9OptimPlotE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN9OptimPlotE = QtMocHelpers::stringData(
    "OptimPlot",
    "detach",
    "",
    "attach",
    "enableCurve",
    "QVariant",
    "itemInfo",
    "on",
    "index",
    "resetZoomBase",
    "QwtPlotItem*",
    "item",
    "plotIsZoomed",
    "options",
    "addMeasuredData",
    "editMeasuredData",
    "fixedLimits",
    "QwtPlot::Axis",
    "axisid",
    "setDisplayStyle",
    "flags",
    "copyToClipboard"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN9OptimPlotE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x06,    1 /* Public */,
       3,    0,   87,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    3,   88,    2, 0x0a,    3 /* Public */,
       4,    2,   95,    2, 0x0a,    7 /* Public */,
       9,    2,  100,    2, 0x0a,   10 /* Public */,
      12,    1,  105,    2, 0x0a,   13 /* Public */,
      13,    0,  108,    2, 0x0a,   15 /* Public */,
      14,    0,  109,    2, 0x0a,   16 /* Public */,
      15,    0,  110,    2, 0x0a,   17 /* Public */,
      16,    1,  111,    2, 0x0a,   18 /* Public */,
      19,    1,  114,    2, 0x0a,   20 /* Public */,
      21,    0,  117,    2, 0x0a,   22 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5, QMetaType::Bool, QMetaType::Int,    6,    7,    8,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int,    6,    8,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Bool,   11,    7,
    QMetaType::Void, QMetaType::QRectF,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject OptimPlot::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtPlot::staticMetaObject>(),
    qt_meta_stringdata_ZN9OptimPlotE.offsetsAndSizes,
    qt_meta_data_ZN9OptimPlotE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN9OptimPlotE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<OptimPlot, std::true_type>,
        // method 'detach'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'attach'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enableCurve'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVariant const &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'enableCurve'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVariant const &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'resetZoomBase'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QwtPlotItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'plotIsZoomed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QRectF const &, std::false_type>,
        // method 'options'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addMeasuredData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'editMeasuredData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fixedLimits'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QwtPlot::Axis, std::false_type>,
        // method 'setDisplayStyle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'copyToClipboard'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void OptimPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OptimPlot *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->detach(); break;
        case 1: _t->attach(); break;
        case 2: _t->enableCurve((*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 3: _t->enableCurve((*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->resetZoomBase((*reinterpret_cast< std::add_pointer_t<QwtPlotItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 5: _t->plotIsZoomed((*reinterpret_cast< std::add_pointer_t<QRectF>>(_a[1]))); break;
        case 6: _t->options(); break;
        case 7: _t->addMeasuredData(); break;
        case 8: _t->editMeasuredData(); break;
        case 9: _t->fixedLimits((*reinterpret_cast< std::add_pointer_t<QwtPlot::Axis>>(_a[1]))); break;
        case 10: _t->setDisplayStyle((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->copyToClipboard(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QwtPlotItem* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (OptimPlot::*)();
            if (_q_method_type _q_method = &OptimPlot::detach; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (OptimPlot::*)();
            if (_q_method_type _q_method = &OptimPlot::attach; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *OptimPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptimPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN9OptimPlotE.stringdata0))
        return static_cast<void*>(this);
    return QwtPlot::qt_metacast(_clname);
}

int OptimPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void OptimPlot::detach()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void OptimPlot::attach()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
