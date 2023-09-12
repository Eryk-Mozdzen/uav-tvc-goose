#pragma once

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QTimer>

namespace widgets {

class LiveChart : public QWidget {
    const qint64 start;
    const float minY;
    const float maxY;
    const float minX;
    bool ignoreSamples;

    QTimer *timer;
    QtCharts::QChart *chart;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY;
    QVector<QtCharts::QLineSeries *> series;

    float getTime() const;

public:
    LiveChart(const float _minY, const float _maxY, QWidget *parent=nullptr);
    void addSeries(const QString name, const QPen pen);

    void append(const QString name, const float value);
    void resume();
    void pause();
};

}
