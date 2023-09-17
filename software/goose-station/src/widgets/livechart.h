#pragma once

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QTimer>

namespace widgets {

class LiveChart : public QWidget {
    const qint64 start;
    bool paused;

    QTimer *timer;
    QtCharts::QChart *chart;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY;
    QVector<QtCharts::QLineSeries *> series;

    float getTime() const;

public:
    struct Config {
        QString title;
        QString yLabel;
        QString yFormat;
        float yMin;
        float yMax;
    };

    LiveChart(const Config &config, QWidget *parent=nullptr);
    void addSeries(const QString name, const QPen pen);

    void append(const QString name, const float value);
    void resume();
    void pause();
};

}
