#pragma once

#include <QMainWindow>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <drake/systems/framework/leaf_system.h>

#include "SharedQueue.h"

class Chart : public QMainWindow, public drake::systems::LeafSystem<double> {
    static constexpr double horizion = 10;
    static constexpr double frequency = 20;
    static constexpr double period = 1/frequency;
    static constexpr double samples = horizion*frequency + 1;

    struct Series {
        QtCharts::QLineSeries *series;
        Eigen::VectorXd selector;
        drake::systems::InputPortIndex port;
        std::unique_ptr<SharedQueue<Eigen::Vector2d>> queue;
    };

    QtCharts::QChart *chart;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY;
    std::vector<Series> series;

    drake::systems::EventStatus update(const drake::systems::Context<double> &context) const;

public:
    Chart(const QString title, const QString yLabel, const QString yFormat, const float yMin, const float yMax);

    void AddSeries(const QString name, const Eigen::VectorXd selector, const QColor color, const Qt::PenStyle style, const int width);
    void AddSeries(const QString name, const Eigen::MatrixXd selector);
};
