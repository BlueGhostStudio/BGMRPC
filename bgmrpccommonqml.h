#ifndef BGMRPCCOMMONQML_H
#define BGMRPCCOMMONQML_H

#include <QObject>
#include <QQmlEngine>

class BGMRPCCommonQml : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(BGMRPCCommon)
    QML_SINGLETON
public:
    explicit BGMRPCCommonQml(QObject* parent = nullptr);

    Q_INVOKABLE QString genObjectName(const QString& grp, const QString& app,
                                      const QString& name,
                                      bool noAppPrefix = false);

signals:
};

#endif  // BGMRPCCOMMONQML_H
