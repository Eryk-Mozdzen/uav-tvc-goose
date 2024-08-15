#pragma once

#include <QObject>

#include <drake/geometry/meshcat.h>

class Visualizer : public QObject {
    Q_OBJECT

    drake::geometry::Meshcat *visualizer;

public slots:
    void receive(const uint8_t id, const QByteArray &payload);

public:
    Visualizer(QObject *parent = nullptr);
    void start();
};
