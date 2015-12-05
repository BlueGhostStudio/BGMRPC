#include "jsobjectprototype.h"
#include "jsplugin.h"
#include <bgmrpc.h>
#include <bgmrobjectstorage.h>

using namespace BGMircroRPCServer;

jsProcProto::jsProcProto(QObject* parent)
    : QObject (parent)
{

}

qulonglong jsProcProto::pID()
{
    return thisProc ()->pID ();
}

void jsProcProto::emitSignal(/*BGMRObjectInterface* */const QScriptValue& _obj,
                             const QString& signal,
                             const QJsonArray& args)
{
    BGMRObjectInterface* obj = engine ()->fromScriptValue < BGMRObjectInterface* > (_obj);
    thisProc ()->emitSignal (obj, signal, args);
}

//void jsProcProto::setPrivateData(const QString& key, const QJsonValue& value)
//{
//    thisProc ()->privateData (key) = value;
//}

//QJsonValue jsProcProto::privateData(const QString& key) const
//{
//    return thisProc ()->privateData (key);
//}

void jsProcProto::setPrivateData(/*BGMRObjectInterface* obj*/const QScriptValue& _obj,
                                 const QString& key, const QJsonValue& value)
{
    BGMRObjectInterface* obj = engine ()->fromScriptValue < BGMRObjectInterface* > (_obj);
    thisProc ()->setPrivateDataJson (obj, key, value);
}

QJsonValue jsProcProto::privateData(/*BGMRObjectInterface**/const QScriptValue& _obj,
                                    const QString& key) const
{
    BGMRObjectInterface* obj = engine ()->fromScriptValue < BGMRObjectInterface* > (_obj);
    return thisProc ()->privateDataJson (obj, key);
}

//bool jsProcProto::isKeepConnected() const
//{
//    return thisProc ()->isKeepConnected ();
//}

QJsonArray jsProcProto::callMethod(const QString& obj,
                                   const QString& method,
                                   const QJsonArray& args)
{
    return thisProc ()->callMethod (obj, method, args);
}

void jsProcProto::close()
{
    thisProc ()->close ();
}

procPtr jsProcProto::thisProc() const
{
    return qvariant_cast < BGMRProcedure* > (thisObject().data().toVariant ());
}


// =======================================================================


jsRPCObjectProto::jsRPCObjectProto(QObject* parent)
    : QObject (parent)
{

}

QString jsRPCObjectProto::objectName() const
{
    return thisRPCObj ()->objectName ();
}

QString jsRPCObjectProto::objectType() const
{
    return thisRPCObj ()->objectType ();
}

QStringList jsRPCObjectProto::objectMethods() const
{
    return thisRPCObj ()->adaptor ()->methods ();
}

QJsonArray jsRPCObjectProto::callMethod(/*BGMRProcedure**/const QScriptValue& _proc,
                                        const QString& method,
                                        const QJsonArray& args)
{
    BGMRProcedure* proc = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
    BGMRObjectInterface* obj = thisRPCObj ();
    return obj->adaptor ()->callMetchod (obj, proc, method, args);
}

BGMRObjectInterface*jsRPCObjectProto::thisRPCObj() const
{
    return qvariant_cast < BGMRObjectInterface* > (thisObject().data().toVariant ());
}



// =======================================================================


jsJsObjProto::jsJsObjProto(QObject* parent)
    :jsRPCObjectProto (parent)
{
//    connect (thisJsObj ()->relProcs (),SIGNAL(removedProc(BGMRProcedure*)),
//             SLOT (test(BGMRProcedure*)));
}

QString jsRPCObjectProto::objectName() const
{
    return thisRPCObj ()->objectName ();
}

QString jsRPCObjectProto::objectType() const
{
    return thisRPCObj ()->objectType ();
}

void jsJsObjProto::addProc(/*BGMRProcedure**/const QScriptValue& _proc)
{
    BGMRProcedure* proc = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
    thisJsObj ()->relProcs ()->addProc (proc);
}

bool jsJsObjProto::removeProc(/*BGMRProcedure**/const QScriptValue& _proc)
{
    BGMRProcedure* proc = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
    if (proc)
        return thisJsObj ()->relProcs ()->removeProc (proc->pID ());
    else
        return false;
}

//bool jsJsObjProto::removeProc(qulonglong pID)
//{
//    return thisJsObj ()->relProcs ()->removeProc (pID);
//}

relProcsMap jsJsObjProto::relProcs() const
{
    return thisJsObj ()->relProcs ()->procs ();
}

BGMRProcedure* jsJsObjProto::relProc(qulonglong pID) const
{
    return thisJsObj ()->relProcs ()->proc (pID);
}

void jsJsObjProto::onRelProcRemoved (const QScriptValue& handel)
{
    qScriptConnect (thisJsObj ()->relProcs (),
                    SIGNAL(removedProc(BGMRProcedure*)), QScriptValue (),
                    handel);
}

bool jsJsObjProto::containsRelProc(/*BGMRProcedure**/const QScriptValue& _proc) const
{
    BGMRProcedure* proc = engine()->fromScriptValue < BGMRProcedure* > (_proc);
    if (proc)
        return thisJsObj ()->relProcs ()->procs ().contains (proc->pID ());
    else
        return false;
}

//bool jsJsObjProto::containsRelProc(qulonglong pID) const
//{
//    return thisJsObj ()->relProcs ()->procs ().contains (pID);
//}

void jsJsObjProto::emitSignal(const QString& signal, const QJsonArray& args) const
{
    thisJsObj ()->relProcs ()->emitSignal (thisRPCObj (), signal, args);
}

void jsJsObjProto::mutexLock()
{
    mutex.lock ();
}

void jsJsObjProto::mutexUnlock()
{
    mutex.unlock ();
}

bool jsJsObjProto::globalMutexLock() const
{
    return thisJsObj ()->globalMutexLock ();
}

void jsJsObjProto::setGlobalMutexLock(bool lock)
{
    thisJsObj ()->setGlobalMutexLock (lock);
}

void jsJsObjProto::loadModule(const QString& module)
{
    thisJsObj ()->loadModule (module);
}

bool jsJsObjProto::loadScript(const QString& scrFileName)
{
    QString jsPluginDir
            = BGMRPC::Settings->value ("pluginDir",
                                       "~/.BGMR/plugins/").toString ();
    jsPluginDir += "/js/";

    QString error;
    bool ok = false;

    QFile scrFile (jsPluginDir + scrFileName);
    if (!scrFile.open (QIODevice::ReadOnly))
        error = QString ("%1 can't open").arg (scrFileName);
    else {
        QString scrContent = scrFile.readAll ();
        scrFile.close ();

        QScriptValue result = engine ()->evaluate (scrContent, scrFileName);
        if (result.isError ()) {
            error = QString ("%0:%1: %2").arg (scrFileName)
                    .arg (result.property ("lineNumber").toInt32 ())
                    .arg (result.toString ());
        } else
            ok = true;
    }

    if (!ok)
        qDebug () << "loadScript Error: " << error;

    return ok;
}

BGMRObjectInterface* jsJsObjProto::thisRPCObj() const
{
    return qvariant_cast < jsObjPtr > (thisObject().data().toVariant ());
}

jsObjPtr jsJsObjProto::thisJsObj() const
{
    return static_cast < jsObjPtr > (thisRPCObj ());
}

// =====================================================================



jsRPC::jsRPC(QObject* parent)
    : QObject (parent)
{

}

RPCObjList jsRPC::objects() const
{
    RPCObjList theObjList;
    foreach (BGMRObjectInterface* obj, RPC->objectStorage ()->objects ())
        theObjList [obj->objectName ()] = obj;

    return theObjList;
}

QStringList jsRPC::types() const
{
    return RPC->objectStorage ()->types ();
}

bool jsRPC::creatorObject(const QString& type, const QString& objName)
{
    return RPC->objectStorage ()->installObject (objName, type);
}

bool jsRPC::removeObject(const QString& objName)
{
    return RPC->objectStorage ()->removeObject (objName);
}

bool jsRPC::installPlugin(const QString& pluginFileName)
{
    return RPC->objectStorage ()->installPlugin (pluginFileName);
}

void jsRPC::setRPC(BGMRPC* rpc)
{
    RPC = rpc;
}

