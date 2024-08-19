#pragma once

#include <QWidget>

#include "QCustomPlot/qcustomplot/qcustomplot.h"

class LiveChart : public QCustomPlot {
    static bool paused;
    static QVector<LiveChart *> registered;

    QString title;
    QVector<QString> series;

public:
    struct Config {
        QString title;
        QString yLabel;
        int yPrecision;
        double yMin;
        double yMax;
        double yTick;
    };

    LiveChart(const Config &config, QWidget *parent=nullptr);
    void addSeries(const QString name, const QPen pen);
    void append(const QString name, const double time, const double value);
    static void synchronize(const double time);
    static void resume();
    static void pause();
    static void save();
};
