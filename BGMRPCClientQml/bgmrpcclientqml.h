#ifndef BGMRPCCLIENTQML_H
#define BGMRPCCLIENTQML_H

#include <QtQuick/QQuickPaintedItem>

class BGMRPCClientQml : public QQuickPaintedItem {
    Q_OBJECT
    QML_ELEMENT
    Q_DISABLE_COPY(BGMRPCClientQml)
public:
    explicit BGMRPCClientQml(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;
    ~BGMRPCClientQml() override;
};

#endif  // BGMRPCCLIENTQML_H
