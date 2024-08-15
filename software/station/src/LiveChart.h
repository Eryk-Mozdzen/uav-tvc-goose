#pragma once

#include <QWidget>
#include <QTimer>

#include "QCustomPlot/qcustomplot/qcustomplot.h"

class LiveChart : public QCustomPlot {
    static qint64 start;
    static bool paused;
    static QVector<LiveChart *> registered;

    QString title;
    QVector<QString> series;
    QTimer *timer;

    static double getTime();

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
    void append(const QString name, const double value);
    static void resume();
    static void pause();
    static void save();
};
