#pragma once

#include <QMainWindow>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <drake/systems/framework/leaf_system.h>

#include "SharedQueue.h"

class GraphXY : public QMainWindow, public drake::systems::LeafSystem<double> {
    static constexpr double horizion = 3;
    static constexpr double frequency = 20;
    static constexpr double period = 1/frequency;
    static constexpr double samples = horizion*frequency;

    struct Series {
        QtCharts::QLineSeries *series;
        Eigen::MatrixXd selector;
        drake::systems::InputPortIndex port;
        std::unique_ptr<SharedQueue<Eigen::Vector2d>> queue;
    };

    QtCharts::QChart *chart;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY;
    std::vector<Series> series;

    drake::systems::EventStatus update(const drake::systems::Context<double> &context) const;

public:
    GraphXY(const QString title, const QString format, const float range);

    void AddSeries(const QString name, const Eigen::MatrixXd selector, const QColor color, const Qt::PenStyle style, const int width);
};
