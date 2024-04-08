#include "calling.h"

Calling::Calling(QObject* parent) : QObject{ parent } {}

void
Calling::resolve(const QJSValue& values) {
    if (m_resolve.isCallable())
        m_resolve.call({ values });
}

void
Calling::reject(const QJSValue& error) {
    if (m_reject.isCallable())
        m_reject.call({ error });
}

Calling*
Calling::then(const QJSValue& _resolve_, const QJSValue& _reject_) {
    m_resolve = _resolve_;
    if (!_reject_.isUndefined())
        m_reject = _reject_;

    return this;
}

Calling*
Calling::_catch(const QJSValue& _reject_) {
    m_reject = _reject_;

    return this;
}
