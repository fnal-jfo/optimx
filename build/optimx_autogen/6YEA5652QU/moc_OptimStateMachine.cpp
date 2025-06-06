/****************************************************************************
** Meta object code from reading C++ file 'OptimStateMachine.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/OptimStateMachine.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptimStateMachine.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17OptimStateMachineE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN17OptimStateMachineE = QtMocHelpers::stringData(
    "OptimStateMachine",
    "editorPresent",
    "",
    "noEditor",
    "subWindowPresent",
    "noSubWindow",
    "latticeLoaded",
    "noLattice",
    "trackerLoaded",
    "noTracker",
    "textEditorLoaded",
    "noTextEditor"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN17OptimStateMachineE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    1 /* Public */,
       3,    0,   75,    2, 0x06,    2 /* Public */,
       4,    0,   76,    2, 0x06,    3 /* Public */,
       5,    0,   77,    2, 0x06,    4 /* Public */,
       6,    0,   78,    2, 0x06,    5 /* Public */,
       7,    0,   79,    2, 0x06,    6 /* Public */,
       8,    0,   80,    2, 0x06,    7 /* Public */,
       9,    0,   81,    2, 0x06,    8 /* Public */,
      10,    0,   82,    2, 0x06,    9 /* Public */,
      11,    0,   83,    2, 0x06,   10 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject OptimStateMachine::staticMetaObject = { {
    QMetaObject::SuperData::link<QStateMachine::staticMetaObject>(),
    qt_meta_stringdata_ZN17OptimStateMachineE.offsetsAndSizes,
    qt_meta_data_ZN17OptimStateMachineE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN17OptimStateMachineE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<OptimStateMachine, std::true_type>,
        // method 'editorPresent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'noEditor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'subWindowPresent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'noSubWindow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'latticeLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'noLattice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'trackerLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'noTracker'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'textEditorLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'noTextEditor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void OptimStateMachine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OptimStateMachine *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->editorPresent(); break;
        case 1: _t->noEditor(); break;
        case 2: _t->subWindowPresent(); break;
        case 3: _t->noSubWindow(); break;
        case 4: _t->latticeLoaded(); break;
        case 5: _t->noLattice(); break;
        case 6: _t->trackerLoaded(); break;
        case 7: _t->noTracker(); break;
        case 8: _t->textEditorLoaded(); break;
        case 9: _t->noTextEditor(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::editorPresent; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::noEditor; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::subWindowPresent; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::noSubWindow; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::latticeLoaded; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::noLattice; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::trackerLoaded; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::noTracker; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::textEditorLoaded; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _q_method_type = void (OptimStateMachine::*)();
            if (_q_method_type _q_method = &OptimStateMachine::noTextEditor; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject *OptimStateMachine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptimStateMachine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN17OptimStateMachineE.stringdata0))
        return static_cast<void*>(this);
    return QStateMachine::qt_metacast(_clname);
}

int OptimStateMachine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStateMachine::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void OptimStateMachine::editorPresent()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void OptimStateMachine::noEditor()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void OptimStateMachine::subWindowPresent()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void OptimStateMachine::noSubWindow()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void OptimStateMachine::latticeLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void OptimStateMachine::noLattice()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void OptimStateMachine::trackerLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void OptimStateMachine::noTracker()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void OptimStateMachine::textEditorLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void OptimStateMachine::noTextEditor()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
