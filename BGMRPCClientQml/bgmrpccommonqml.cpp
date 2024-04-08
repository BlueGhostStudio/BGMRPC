#include "bgmrpccommonqml.h"

BGMRPCCommonQml::BGMRPCCommonQml(QObject* parent) : QObject{ parent } {}

QString
BGMRPCCommonQml::genObjectName(const QString& grp, const QString& app,
                               const QString& name, bool noAppPrefix) {
    if (noAppPrefix)
        return name;
    else if (!grp.isEmpty())
        return grp + "::" + app + "::" + name;
    else
        return (!app.isEmpty() ? app + "::" : "") + name;
}
