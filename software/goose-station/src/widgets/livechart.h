#pragma once

#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>

namespace widgets {

class LiveChart : public QWidget {
    const qint64 start;
    const float minY;
    const float maxY;

    QtCharts::QChart *chart;
    QtCharts::QChartView *view;
    QVector<QtCharts::QLineSeries *> series;

    float getTime() const;

public:
    LiveChart(const float _minY, const float _maxY, QWidget *parent=nullptr);
    void addSeries(const QString name, const QPen pen);

    void append(const QString name, const float value);
};

}
