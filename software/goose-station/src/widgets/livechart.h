#pragma once

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QTimer>

namespace widgets {

class LiveChart : public QWidget {
    static const qint64 start;
    static bool paused;
    static QVector<QtCharts::QLineSeries *> series;

    QTimer *timer;
    QtCharts::QChart *chart;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY;

    static float getTime();

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
    static void resume();
    static void pause();
};

}
