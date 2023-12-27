#include <bgmrpccommon.h>
#include <getopt.h>

#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QLocalSocket>
#include <QProcess>
#include <QSettings>
#include <QTimer>
#include <functional>

bool serverRunning = false;
QLocalSocket ctrlSocket;
QString binPath;

void
startServer(int argc, char* argv[]) {
    if (serverRunning) {
        qWarning().noquote() << "Server already running";
        return;
    }

    QSettings* settings;
    //    QString startCmd;
    QStringList args;
    QString rootPath;
    QString logPath;
    QString binPath;

    QProcess BGMRPCProcess;

    if (argc >= 3) {
        args << "-c" << argv[2];
        settings = new QSettings(argv[2], QSettings::IniFormat);
    } else {
        settings = new QSettings();
    }

    rootPath = settings->value("path/root").toString();
    rootPath.replace(QRegularExpression("^~"), QDir::homePath());
    logPath = settings->value("path/logs", rootPath + "/logs").toString() +
              "/BGMRPC.log";
    logPath.replace(QRegularExpression("^~"), QDir::homePath());
    binPath = settings->value("path/bin", rootPath + "/bin").toString();
    binPath.replace(QRegularExpression("^~"), QDir::homePath());

    if (qgetenv("BGMRPCDebug") != "1") {
        BGMRPCProcess.setStandardOutputFile(logPath /*, QIODevice::Append*/);
        BGMRPCProcess.setStandardErrorFile(logPath /*, QIODevice::Append*/);
    }

    BGMRPCProcess.setProgram(binPath + "/BGMRPCd");
    BGMRPCProcess.setArguments(args);
    BGMRPCProcess.startDetached();
}

void
stopServer() {
    if (serverRunning) {
        QByteArray cmd(1, NS_BGMRPC::CTRL_STOPSERVER);
        qInfo().noquote() << "BGMRPC,stopServer,Stoping...";
        ctrlSocket.write(cmd);
        if (ctrlSocket.waitForBytesWritten())
            qInfo().noquote() << "BGMRPC,stopServer,Stoped";
    }
}

bool
checkObject(const QByteArray& objName) {
    QByteArray checkObject(1, NS_BGMRPC::CTRL_CHECKOBJECT);
    checkObject.append(objName);
    ctrlSocket.write(checkObject);
    if (!ctrlSocket.waitForBytesWritten() || !ctrlSocket.waitForReadyRead()) {
        qWarning().noquote()
            << "Object,createObject-checkObject,Can't check object";
        return false;
    } else if ((quint8)ctrlSocket.readAll()[0] == 1) {
        qWarning().noquote() << QString(
                                    "Object,createObject-checkObject,The "
                                    "Object(%1) already existed")
                                    .arg(objName);
        return true;
    } else
        return false;
}

bool
createObject(const QByteArray& name, const QStringList& args) {
    if (checkObject(name)) return true;

    QProcess loaderProcess;
    QString logPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_LOGS) + "/" +
                      QString(args[0]) + ".log";
    QString binPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_BIN);

    if (qgetenv("BGMRPCDebug") != "1") {
        loaderProcess.setStandardOutputFile(logPath /*, QIODevice::Append*/);
        loaderProcess.setStandardErrorFile(logPath /*, QIODevice::Append*/);
    }

    loaderProcess.setProgram(binPath + (QT_VERSION > 0X060000
                                            ? "/BGMRPCObjectLoader"
                                            : "/BGMRPCObjectLoader-qt5"));
    loaderProcess.setArguments(args);
    loaderProcess.startDetached();

    bool ok = false;
    QThread::msleep(1000);
    ok = checkObject(name);

    qInfo().noquote() << "CreateObject " << (ok ? "ok" : "fail");

    return ok;
}

bool
createObject(int argc, char* argv[]) {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,createObject,Server not run";
        return false;
    } else if (argc < 4) {
        qWarning().noquote() << "Object,createObject,Mistake arguments";
        return false;
    }

    QByteArray app;
    QByteArray group;
    bool noAppPrefix = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, "g:a:A")) != -1) {
        switch (opt) {
        case 'g':
            group = optarg;
            break;
        case 'a':
            app = optarg;
            break;
        case 'A':
            noAppPrefix = true;
            break;
        case '?':
            break;
        }
    }

    QByteArray objName = genObjectName(group, app, argv[2], noAppPrefix);

    QStringList args;
    args << "-n" << argv[2] << "-I" << argv[3];
    for (int i = 4; i < argc; i++) args << argv[i];

    return createObject(objName, args);
}

bool
detachObject(const QByteArray& obj) {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,detachObject,server not run";
        return false;
    }

    qInfo().noquote()
        << QString("Object(%1),detachObject,Detach object...").arg(obj);

    QByteArray cmd(1, NS_BGMRPC::CTRL_DETACHOBJECT);
    cmd.append(obj);
    ctrlSocket.write(cmd);
    if (ctrlSocket.waitForReadyRead() && (quint8)ctrlSocket.readAll()[0])
        qInfo().noquote()
            << QString("Object(%1),detachObject,Finished").arg(obj);
    else
        qWarning().noquote()
            << QString("Object(%1),detachObject,Detach Fail").arg(obj);

    return true;
}

bool
detachObject(int argc, char* argv[]) {
    if (argc < 2) {
        qWarning().noquote() << "Object,detachObject,Mistake arguments";
        return false;
    }

    return detachObject(argv[2]);
}

void
listObjects() {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,listObjects,Server not run";
        return;
    }

    QByteArray cmd(1, NS_BGMRPC::CTRL_LISTOBJECTS);
    ctrlSocket.write(cmd);
    if (!ctrlSocket.waitForBytesWritten() || !ctrlSocket.waitForReadyRead()) {
        qWarning().noquote() << "BGMRPC,listObjects,Can't get object list";
        return;
    }

    QByteArray objListData = ctrlSocket.readAll();
    if (objListData[0] != '\x0') {
        foreach (const QByteArray obj, objListData.split(','))
            qInfo().noquote() << "- " << obj;
    }
}

void
listApps() {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,listObjects,Server not run";
        return;
    }

    QByteArray rootPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT);
    if (rootPath.isEmpty()) return;

    QDir appsDir(rootPath + "/apps");
    foreach (const QString& app,
             appsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        qInfo().noquote() << "- " << app;
    }
}

int
iteratorRelApps(
    const QString& grp, const QJsonArray& jsaRelApps,
    std::function<bool(const QString&, const QString&, const QJsonValue&)> cb) {
    foreach (const QJsonValue& jsvRelApp, jsaRelApps) {
        QString relAppName;
        QString relAppGrp = grp;

        if (jsvRelApp.isObject()) {
            QJsonObject jsoRelApp = jsvRelApp.toObject();
            relAppName = jsoRelApp["app"].toString("");

            if (jsoRelApp.contains("grp")) {
                relAppGrp = jsoRelApp["grp"].toString();
                if (relAppGrp.isEmpty()) relAppGrp = grp;
            }
        } else
            relAppName = jsvRelApp.toString("");

        if (relAppName.isEmpty()) continue;

        if (!cb(relAppName, relAppGrp, jsvRelApp)) return false;
    }

    return true;
}

bool
processAppJson(
    const QString& app, const QString& grp,
    std::function<bool(const QString&, const QString&, const QJsonValue&)>
        requiredAppsCB,
    std::function<bool(const QString&, const QString&, const QJsonValue&)>
        optionalAppsCB,
    std::function<bool(const QString&, const QString&, bool,
                       const QJsonObject&)>
        objectCB,
    bool required) {
    QByteArray rootPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT);
    if (rootPath.isEmpty()) return false;

    QDir appDir(rootPath + "/apps/" + app);
    if (!appDir.exists() || !appDir.exists("app.json")) return false;

    QFile appJsonFile(appDir.filePath("app.json"));
    if (!appJsonFile.open(QIODevice::ReadOnly)) return false;

    QByteArray jsonData = appJsonFile.readAll();
    QJsonDocument appJson = QJsonDocument::fromJson(jsonData);
    appJsonFile.close();

    QString group = grp.isEmpty() ? appJson["grp"].toString("") : grp;

    if (!iteratorRelApps(group, appJson["required-apps"].toArray({}),
                         requiredAppsCB) &&
        required)
        return false;

    iteratorRelApps(group, appJson["optional-apps"].toArray({}),
                    optionalAppsCB);

    QJsonArray jsaObjs = appJson["objs"].toArray({});
    foreach (const QJsonValue& jsvObj, jsaObjs) {
        QJsonObject jsoObj = jsvObj.toObject({});

        QString objName = jsoObj["obj"].toString("");
        if (objName.isEmpty()) continue;

        if (!objectCB(group, objName, jsoObj["noprefix"].toBool(false),
                      jsoObj) &&
            required)
            return false;
    }
    return true;
}

bool
runApp(const QString& app, const QString& grp) {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,listObjects,Server not run";
        return false;
    }

    qInfo().noquote() << "Starting " + app + " ...";

    processAppJson(
        app, grp,
        [](const QString& relApp, const QString& relAppGrp,
           const QJsonValue&) -> bool { return runApp(relApp, relAppGrp); },
        [](const QString& relApp, const QString& relAppGrp,
           const QJsonValue&) -> bool {
            runApp(relApp, relAppGrp);
            return true;
        },
        [app](const QString& grp, const QString& objName, bool noprefix,
              const QJsonObject& jsoObj) -> bool {
            QString interface = jsoObj["IF"].toString("");
            if (interface.isEmpty()) return false;

            QStringList args({ "-n", objName, "-I", interface, "-a", app });
            if (noprefix) args << "-A";
            if (!grp.isEmpty()) args << "-g" << grp;
            args << QProcess::splitCommand(jsoObj["args"].toString(""));

            return createObject(genObjectName(grp.toLatin1(), app.toLatin1(),
                                              objName.toLatin1(), noprefix),
                                args);
        },
        true);

    /*QByteArray rootPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT);
    if (rootPath.isEmpty()) return false;

    QDir appDir(rootPath + "/apps/" + app);
    if (!appDir.exists() || !appDir.exists("app.json")) return false;

    QFile appJsonFile(appDir.filePath("app.json"));
    if (!appJsonFile.open(QIODevice::ReadOnly)) return false;

    qInfo().noquote() << "Starting " + app + " ...";

    QByteArray jsonData = appJsonFile.readAll();
    QJsonDocument appJson = QJsonDocument::fromJson(jsonData);
    appJsonFile.close();

    QString group = grp.isEmpty() ? appJson["grp"].toString("") : grp;

    if (!iteratorRelApps(group, appJson["required-apps"].toArray({}),
                         [](const QString& relApp, const QString& relAppGrp,
                            const QJsonValue&) -> bool {
                             return runApp(relApp, relAppGrp);
                         }))
        return false;

    iteratorRelApps(group, appJson["optional-apps"].toArray({}),
                    [](const QString& relApp, const QString& relAppGrp,
                       const QJsonValue&) -> bool {
                        runApp(relApp, relAppGrp);
                        return true;
                    });

    QJsonArray jaObjs = appJson["objs"].toArray({});
    foreach (const QJsonValue& jvObj, jaObjs) {
        QJsonObject joObj = jvObj.toObject({});

        QString objName = joObj["obj"].toString("");
        if (objName.isEmpty()) continue;

        QString interface = joObj["IF"].toString("");
        if (interface.isEmpty()) return false;

        bool noprefix = joObj["noprefix"].toBool(false);

        QStringList createObjArgs(
            { "-n", objName, "-I", interface, "-a", app });
        if (noprefix) createObjArgs << "-A";
        if (!group.isEmpty()) createObjArgs << "-g" << group;
        createObjArgs << QProcess::splitCommand(joObj["args"].toString(""));

        if (!createObject(genObjectName(group.toLatin1(), app.toLatin1(),
                                        objName.toLatin1(), noprefix),
                          createObjArgs))
            return false;
    }*/

    qDebug().noquote() << "\n\n";

    return true;
}

bool
stopApp(const QString& app, const QString& grp) {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC, listObjects, Server not run";
        return false;
    }

    qInfo().noquote() << "Stopping " + app + " ...";

    auto stopRelApps = [](const QString& relApp, const QString& relAppGrp,
                          const QJsonValue& jsvRelApp) -> bool {
        if (!jsvRelApp.isObject() ||
            !jsvRelApp.toObject({})["skip-stop"].toBool(false))
            stopApp(relApp, relAppGrp);

        return true;
    };

    processAppJson(
        app, grp, stopRelApps, stopRelApps,
        [app](const QString& grp, const QString& objName, bool noprefix,
              const QJsonObject& jsoObj) -> bool {
            detachObject(genObjectName(grp.toLatin1(), app.toLatin1(),
                                       objName.toLatin1(), noprefix));
            return true;
        },
        false);

    qDebug().noquote() << "\n\n";

    return true;
}

int
main(int argc, char* argv[]) {
    QCoreApplication::setSetuidAllowed(true);

    QCoreApplication a(argc, argv);

    QString usage =
        "commands: start, stop, createObject, detachObject, listObjects";

    if (argc < 2) {
        qWarning().noquote() << usage;
        return -1;
    }

    ctrlSocket.connectToServer(BGMRPCServerCtrlSocket);

    if (ctrlSocket.waitForConnected(500)) serverRunning = true;

    if (serverRunning)
        binPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_BIN);

    if (strcmp(argv[1], "start") == 0)
        startServer(argc, argv);
    else if (strcmp(argv[1], "stop") == 0)
        stopServer();
    else if (strcmp(argv[1], "createObject") == 0)
        createObject(argc, argv);
    else if (strcmp(argv[1], "detachObject") == 0)
        detachObject(argc, argv);
    else if (strcmp(argv[1], "listObjects") == 0)
        listObjects();
    else if (strcmp(argv[1], "listApps") == 0)
        listApps();
    else if (strcmp(argv[1], "runApp") == 0 && argc > 2)
        runApp(argv[2], argc > 3 ? argv[3] : "");
    else if (strcmp(argv[1], "stopApp") == 0 && argc > 2)
        stopApp(argv[2], argc > 3 ? argv[3] : "");
    else
        qWarning().noquote() << usage;
}
