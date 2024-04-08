#include "bgmrpcclientqml.h"

#include <QSslConfiguration>
#include <QTimer>

quint64 BGMRPCClientQml::m_totalMID = 0;

BGMRPCClientQml::BGMRPCClientQml(QObject* parent) : QObject(parent) {
    QObject::connect(
        &m_socket, &QWebSocket::textMessageReceived, this,
        [=](const QString& message) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
            QString type = jsonDoc["type"].toString();
            if (type == "signal")
                emit remoteSignal(jsonDoc["object"].toString(),
                                  jsonDoc["signal"].toString(),
                                  qjsEngine(this)->toScriptValue<QJsonValue>(
                                      jsonDoc["args"]));
            else
                emit dataReady(jsonDoc);
        });
    QObject::connect(&m_socket, &QWebSocket::stateChanged, this,
                     &BGMRPCClientQml::isConnectedChanged);
    QObject::connect(&m_socket, &QWebSocket::stateChanged, this,
                     &BGMRPCClientQml::stateChanged);
    QObject::connect(&m_socket, &QWebSocket::errorOccurred, this,
                     &BGMRPCClientQml::error);
    QObject::connect(&m_socket, &QWebSocket::disconnected, this, [=]() {
        QObject::disconnect(this, &BGMRPCClientQml::dataReady, nullptr,
                            nullptr);
    });
}

BGMRPCClientQml::~BGMRPCClientQml() {}

void
BGMRPCClientQml::active() {
    if (!m_url.isEmpty()) {
        if (m_url.scheme() == "wss") {
            QSslConfiguration conf;
            conf.setPeerVerifyMode(QSslSocket::VerifyNone);
            m_socket.setSslConfiguration(conf);
        }

        m_socket.open(m_url);
    }
}

void
BGMRPCClientQml::inactive() {
    m_socket.close();
}

Calling*
BGMRPCClientQml::callMethod(const QString& object, const QString& method,
                            const QJSValue& args) {
    QString mID = QString("#%1").arg(m_totalMID);
    m_totalMID++;

    QJsonObject jsoCall;
    jsoCall["object"] = object;
    jsoCall["method"] = method;
    jsoCall["args"] = qjsEngine(this)->fromScriptValue<QJsonArray>(args);
    jsoCall["mID"] = mID;

    Calling* theCalling = new Calling();
    QMetaObject::Connection* __connection__ = new QMetaObject::Connection;
    *__connection__ = QObject::connect(
        this, &BGMRPCClientQml::dataReady, this,
        [=](const QJsonDocument& jsonDoc) {
            QString type = jsonDoc["type"].toString();

            if (jsonDoc["mID"] == mID) {
                QObject::disconnect(*__connection__);
                delete __connection__;
            }

            if (type == "return")
                theCalling->resolve(qjsEngine(this)->toScriptValue<QJsonValue>(
                    jsonDoc.object()));
            else if (type == "error")
                theCalling->reject(qjsEngine(this)->toScriptValue<QJsonValue>(
                    jsonDoc.object()));
        });
    /**__connection__ = QObject::connect(
        &m_socket, &QWebSocket::textMessageReceived, this,
        [=](const QString& message) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
            QString type = jsonDoc["type"].toString();
            if (type == "signal")
                emit remoteSignal(jsonDoc["object"].toString(),
                                  jsonDoc["signal"].toString(),
                                  qjsEngine(this)->toScriptValue<QJsonValue>(
                                      jsonDoc["args"]));
            else if (jsonDoc["mID"] == mID) {
                QObject::disconnect(*__connection__);
                delete __connection__;

                if (type == "return")
                    theCalling->resolve(
                        qjsEngine(this)->toScriptValue<QJsonValue>(
                            jsonDoc.object()));
                else if (type == "error")
                    theCalling->reject(
                        qjsEngine(this)->toScriptValue<QJsonValue>(
                            jsonDoc.object()));
            }
        });*/

    m_socket.sendTextMessage(QJsonDocument(jsoCall).toJson());

    return theCalling;
}

QUrl
BGMRPCClientQml::url() const {
    return m_url;
}

void
BGMRPCClientQml::setUrl(const QUrl& newUrl) {
    if (m_url == newUrl) return;
    m_url = newUrl;
    emit urlChanged();
}

bool
BGMRPCClientQml::isConnected() const {
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

BGMRPCClientQml::SOCKSTATE
BGMRPCClientQml::state() const {
    return (BGMRPCClientQml::SOCKSTATE)m_socket.state();
}

QString
BGMRPCClientQml::errorString() const {
    return m_socket.errorString();
}
