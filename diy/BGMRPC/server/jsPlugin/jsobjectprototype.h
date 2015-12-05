#ifndef JSOBJECTPROTOTYPE_H
#define JSOBJECTPROTOTYPE_H

#include <QObject>
#include <QtScript>
#include <bgmrprocedure.h>
#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <bgmrobjectstorage.h>
#include <QtSql/QtSql>
#include <qsqldatabase.h>
#include <QFile>

#include "jsmetatype.h"
#include "jsobjectclass.h"

using namespace BGMircroRPCServer;

typedef BGMRProcedure* procPtr;

class jsProcProto : public QObject, public QScriptable
{
    Q_PROPERTY( qulonglong pID READ pID )
//    Q_PROPERTY(bool isKeepConnected READ isKeepConnected)

Q_OBJECT
public:
    jsProcProto (QObject* parent = 0);

    qulonglong pID ();

    Q_INVOKABLE void emitSignal (/*BGMRObjectInterface**/const QScriptValue& _obj,
                                 const QString& signal,
                                 const QJsonArray& args);
//    Q_INVOKABLE void setPrivateData (const QString& key,
//                                     const QJsonValue& value);
//    Q_INVOKABLE QJsonValue privateData (const QString& key) const;
    Q_INVOKABLE void setPrivateData (/*BGMRObjectInterface* obj*/const QScriptValue& _obj,
                                     const QString& key,
                                     const QJsonValue& value);
    Q_INVOKABLE QJsonValue privateData (const QScriptValue& obj,
                                        const QString& key) const;
//    Q_INVOKABLE bool isKeepConnected () const;
    Q_INVOKABLE QJsonArray callMethod (const QString& obj,
                                       const QString& method,
                                       const QJsonArray& args);
    Q_INVOKABLE void close ();

private:
    procPtr thisProc() const;
};

template <>
class protoTypeInfo < jsProcProto >
{
public:
    typedef procPtr dataType;
    static QString className () { return "BGMRProcedure"; }
    static bool isNull (dataType d) { return !d; }
    static procPtr nullData () { return NULL; }
    static dataType newObject () { return NULL; }
};

typedef jsObjectClass < jsProcProto > jsProcClass;

Q_DECLARE_METATYPE (const BGMRProcedure*)
Q_DECLARE_METATYPE (BGMRProcedure*)
Q_DECLARE_METATYPE (jsProcClass*)

// ===========================================================

class jsRPCObjectProto;
typedef BGMRObjectInterface* rpcObjPtr;

template <>
class protoTypeInfo < jsRPCObjectProto >
{
public:
    typedef rpcObjPtr dataType;
    static QString className () { return "BGMRObject"; }
    static bool isNull (dataType d) { return !d; }
    static rpcObjPtr nullData () { return NULL; }
    static dataType newObject () { return NULL; }
};

class jsRPCObjectProto : public QObject, public QScriptable
{
    Q_PROPERTY(QString __NAME__ READ objectName)
    Q_PROPERTY(QString __TYPE__ READ objectType)
    Q_PROPERTY(QStringList __METHODS__ READ objectMethods)
    Q_OBJECT
public:
    jsRPCObjectProto (QObject* parent = 0);

    Q_INVOKABLE QString objectName () const;
    Q_INVOKABLE QString objectType () const;
    Q_INVOKABLE QStringList objectMethods () const;
    Q_INVOKABLE QJsonArray callMethod (const QScriptValue& _proc,
                                       const QString& method,
                                       const QJsonArray& args);

protected:
    virtual BGMRObjectInterface* thisRPCObj () const;
};

typedef jsObjectClass < jsRPCObjectProto > jsRPCObjectClass;
Q_DECLARE_METATYPE (const BGMRObjectInterface*)
Q_DECLARE_METATYPE (BGMRObjectInterface*)
Q_DECLARE_METATYPE (jsRPCObjectClass*)

// ===========================================================

class jsObj;
class jsJsObjProto;
typedef jsObj* jsObjPtr;

template <>
class protoTypeInfo < jsJsObjProto >
{
public:
    typedef jsObjPtr dataType;
    static QString className () { return "jsObj"; }
    static bool isNull (dataType d) { return !d; }
    static jsObjPtr nullData () { return NULL; }
    static dataType newObject () { return NULL; }
};

class jsJsObjProto : public jsRPCObjectProto
{
    Q_PROPERTY(QString __NAME__ READ objectName)
    Q_PROPERTY(QString __TYPE__ READ objectType)
    Q_PROPERTY(relProcsMap relProcs READ relProcs)
    Q_PROPERTY(bool globalMutexLock READ globalMutexLock WRITE setGlobalMutexLock)
    Q_OBJECT
public:
    jsJsObjProto(QObject* parent = 0);

    Q_INVOKABLE QString objectName () const;
    Q_INVOKABLE QString objectType () const;

    Q_INVOKABLE void addProc (const QScriptValue& _proc);
//    Q_INVOKABLE bool removeProc (qulonglong pID);
    Q_INVOKABLE bool removeProc (const QScriptValue& _proc);
    Q_INVOKABLE relProcsMap relProcs () const;
    Q_INVOKABLE BGMRProcedure* relProc (qulonglong pID) const;
    Q_INVOKABLE void onRelProcRemoved (const QScriptValue& handel);
//    Q_INVOKABLE bool containsRelProc (qulonglong pID) const;
    Q_INVOKABLE bool containsRelProc (const QScriptValue& _proc) const;
    Q_INVOKABLE void emitSignal (const QString& signal,
                                 const QJsonArray& args) const;
    Q_INVOKABLE void mutexLock ();
    Q_INVOKABLE void mutexUnlock ();
    Q_INVOKABLE bool globalMutexLock () const;
    Q_INVOKABLE void setGlobalMutexLock (bool lock);
    Q_INVOKABLE void loadModule (const QString& module);
    Q_INVOKABLE bool loadScript(const QString& scrFileName);

protected:
    BGMRObjectInterface* thisRPCObj () const;
    jsObjPtr thisJsObj () const;
};

typedef jsObjectClass < jsJsObjProto > jsJsObjClass;
Q_DECLARE_METATYPE (jsJsObjClass*)


// ===========================================================

class jsRPC : public QObject
{
    Q_PROPERTY(RPCObjList objects READ objects)
    Q_PROPERTY(QStringList types READ types)
    Q_OBJECT
public:
    jsRPC (QObject* parent = 0);

    Q_INVOKABLE RPCObjList objects () const;
    Q_INVOKABLE QStringList types () const;
    Q_INVOKABLE bool creatorObject (const QString& type, const QString& objName);
    Q_INVOKABLE bool removeObject (const QString& objName);
    Q_INVOKABLE bool installPlugin (const QString& pluginFileName);
    void setRPC (BGMRPC* rpc);

private:
    BGMRPC* RPC;
};

#endif // JSOBJECTPROTOTYPE_H
