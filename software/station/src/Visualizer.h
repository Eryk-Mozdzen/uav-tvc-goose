#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QTcpSocket>
#include <QSettings>

class Visualizer : public QGroupBox {
    Q_OBJECT

    double cameraPosition[3] = {0, 0, 0};

    QTcpSocket socket;
    QPushButton *spawnButton;
    QSettings settings;

    void write(const char *format, ...);

public slots:
    void receive(const uint8_t id, const double time, const QByteArray &payload);

public:
    Visualizer(QWidget *parent = nullptr);
};
