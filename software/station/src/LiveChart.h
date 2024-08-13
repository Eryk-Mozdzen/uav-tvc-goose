#pragma once

#include <QWidget>
#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QTimer>

class LiveChart : public QChartView {
    static qint64 start;
    static bool paused;
    static QVector<QLineSeries *> series;

    QTimer *timer;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;

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
    static void save();
};
