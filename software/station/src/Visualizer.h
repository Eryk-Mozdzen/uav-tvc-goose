#pragma once

#include <QObject>
#include <QTcpSocket>

class Visualizer : public QObject {
    Q_OBJECT

    double cameraPosition[3] = {0, 0, 0};
    float latlon_ref[2] = {0, 0};

    QTcpSocket *socket = nullptr;

    void write(const char *format, ...);

public slots:
    void receive(const uint8_t id, const double time, const QByteArray &payload);
    void spawnLight();
    void spawnDark();

signals:
    void finished();

public:
    Visualizer(QObject *parent = nullptr);
    void start();
};
