/****************************************************************************
** Meta object code from reading C++ file 'smtp_server.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/smtp_server.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'smtp_server.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SMTP_Client_t {
    QByteArrayData data[7];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SMTP_Client_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SMTP_Client_t qt_meta_stringdata_SMTP_Client = {
    {
QT_MOC_LITERAL(0, 0, 11), // "SMTP_Client"
QT_MOC_LITERAL(1, 12, 18), // "signal_unconnected"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 17), // "slot_stateChanged"
QT_MOC_LITERAL(4, 50, 28), // "QAbstractSocket::SocketState"
QT_MOC_LITERAL(5, 79, 5), // "state"
QT_MOC_LITERAL(6, 85, 14) // "slot_readyRead"

    },
    "SMTP_Client\0signal_unconnected\0\0"
    "slot_stateChanged\0QAbstractSocket::SocketState\0"
    "state\0slot_readyRead"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SMTP_Client[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   30,    2, 0x08 /* Private */,
       6,    0,   33,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,

       0        // eod
};

void SMTP_Client::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SMTP_Client *_t = static_cast<SMTP_Client *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signal_unconnected(); break;
        case 1: _t->slot_stateChanged((*reinterpret_cast< const QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 2: _t->slot_readyRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketState >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SMTP_Client::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SMTP_Client::signal_unconnected)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject SMTP_Client::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SMTP_Client.data,
      qt_meta_data_SMTP_Client,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SMTP_Client::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SMTP_Client::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SMTP_Client.stringdata0))
        return static_cast<void*>(const_cast< SMTP_Client*>(this));
    return QObject::qt_metacast(_clname);
}

int SMTP_Client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void SMTP_Client::signal_unconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_SMTP_Server_t {
    QByteArrayData data[3];
    char stringdata0[38];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SMTP_Server_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SMTP_Server_t qt_meta_stringdata_SMTP_Server = {
    {
QT_MOC_LITERAL(0, 0, 11), // "SMTP_Server"
QT_MOC_LITERAL(1, 12, 24), // "slot_incommingConnection"
QT_MOC_LITERAL(2, 37, 0) // ""

    },
    "SMTP_Server\0slot_incommingConnection\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SMTP_Server[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void SMTP_Server::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SMTP_Server *_t = static_cast<SMTP_Server *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slot_incommingConnection(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SMTP_Server::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SMTP_Server.data,
      qt_meta_data_SMTP_Server,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SMTP_Server::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SMTP_Server::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SMTP_Server.stringdata0))
        return static_cast<void*>(const_cast< SMTP_Server*>(this));
    return QObject::qt_metacast(_clname);
}

int SMTP_Server::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
