#ifndef BGMRPCCLIENTQML_H
#define BGMRPCCLIENTQML_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QQuickItem>
#include <QWebSocket>

#include "calling.h"

class BGMRPCClientQml : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(BGMRPC)
    Q_DISABLE_COPY(BGMRPCClientQml)

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged FINAL)
    Q_PROPERTY(
        bool isConnected READ isConnected NOTIFY isConnectedChanged FINAL)
    Q_PROPERTY(SOCKSTATE state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(QString errorString READ errorString CONSTANT FINAL)

public:
    explicit BGMRPCClientQml(QObject* parent = nullptr);
    ~BGMRPCClientQml() override;

    enum SOCKSTATE {
        UnconnectedState = QAbstractSocket::UnconnectedState,
        ConnectingState = QAbstractSocket::ConnectingState,
        ConnectedState = QAbstractSocket::ConnectedState,
        ClosingState = QAbstractSocket::ClosingState
    };
    Q_ENUM(SOCKSTATE)

    Q_INVOKABLE void active();
    Q_INVOKABLE void inactive();
    Q_INVOKABLE Calling* callMethod(const QString& object,
                                    const QString& method,
                                    const QJSValue& args);

    QUrl url() const;
    void setUrl(const QUrl& newUrl);

    bool isConnected() const;

    SOCKSTATE state() const;

    QString errorString() const;

signals:
    void dataReady(const QJsonDocument& jsonDoc);

    void remoteSignal(const QString& obj, const QString& sig,
                      const QJSValue& args);

    void urlChanged();
    void isConnectedChanged();
    void stateChanged();
    void error();

private:
    QUrl m_url;
    QWebSocket m_socket;
    static quint64 m_totalMID;
};

#endif  // BGMRPCCLIENTQML_H
