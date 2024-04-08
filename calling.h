#ifndef CALLING_H
#define CALLING_H

#include <QObject>
#include <QQmlEngine>

class Calling : public QObject {
    Q_OBJECT
    // QML_ELEMENT
public:
    explicit Calling(QObject* parent = nullptr);

    void resolve(const QJSValue& values);
    void reject(const QJSValue& error);

    Q_INVOKABLE Calling* then(const QJSValue& _resolve_,
                              const QJSValue& _reject_ = QJSValue());
    Q_INVOKABLE Calling* _catch(const QJSValue& _reject_);

signals:

private:
    QJSValue m_resolve;
    QJSValue m_reject;
};

#endif  // CALLING_H
