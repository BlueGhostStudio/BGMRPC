#ifndef BGMRPCCLIENT_H
#define BGMRPCCLIENT_H

#include "BGMRPCClient_global.h"
#include "CallChain.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QWebSocket>

namespace NS_BGMRPCClient {

class BGMRPCClient;

class Calling : public QObject {
    Q_OBJECT
public:
    Calling(const QString& mID, QObject* parent = nullptr);

    void waitForReturn(CallChain* callChain, const BGMRPCClient* client);

private:
    QString m_mID;
};

class BGMRPCCLIENT_EXPORT BGMRPCClient : public QObject {
    Q_OBJECT
public:
    BGMRPCClient(QObject* parent = nullptr);

    void connectToHost(const QUrl& url);

signals:
    void connected();
    void disconnected();
    void onReturn(const QJsonDocument& jsonDoc);
    void onRemoteSignal(const QString& obj, const QString& sig,
                        const QJsonArray& args);
    void onError(const QJsonDocument& error);

public:
    void callMethod(CallChain* callChain, const QString& object,
                    const QString& method, const QVariantList& args);

protected:
    QWebSocket m_socket;
    QString m_mID;
    static quint64 m_totalMID;
};
} // namespace NS_BGMRPCClient
#endif // BGMRPCCLIENT_H
