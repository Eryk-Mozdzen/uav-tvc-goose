#pragma once

#include <QTcpSocket>
#include <QTimer>
#include <drake/systems/framework/leaf_system.h>

#include "SharedQueue.h"

class VisualClient : public drake::systems::LeafSystem<double> {
    static constexpr double frequency = 20;
    static constexpr double period = 1/frequency;
    static constexpr int port = 8080;
    static constexpr double rad2deg = 57.29578;

    std::unique_ptr<SharedQueue<Eigen::VectorXd>> queue;
    QTcpSocket socket;
    QTimer timer;

    void write(const QString &message);

    drake::systems::EventStatus update(const drake::systems::Context<double> &context) const;

public:
    VisualClient();
};
