#include "livechart.h"
#include <QDateTime>
#include <QDebug>
#include <QChartView>

namespace widgets {

LiveChart::LiveChart(const float _minY, const float _maxY, QWidget *parent) : QWidget{parent},
        start{QDateTime::currentMSecsSinceEpoch()},
        minY{_minY},
        maxY{_maxY},
        minX{-10},
        ignoreSamples{false} {
    setFixedSize(500, 320);

    chart = new QtCharts::QChart();
    chart->legend()->hide();

    QtCharts::QChartView *view = new QtCharts::QChartView(chart, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setGeometry(0, 0, width(), height());

    axisX = new QtCharts::QValueAxis(this);
    axisY = new QtCharts::QValueAxis(this);
    axisX->setRange(minX, 0);
    axisY->setRange(minY, maxY);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    timer = new QTimer(this);
    timer->setInterval(20);
    timer->start();

    connect(timer, &QTimer::timeout, [this]() {
        const float t = getTime();

        axisX->setRange(minX+t, t);
        axisY->setRange(minY, maxY);
    });
}

void LiveChart::addSeries(const QString name, const QPen pen) {
    QtCharts::QLineSeries *s = new QtCharts::QLineSeries(this);

    chart->addSeries(s);

    s->setPen(pen);
    s->setName(name);
    s->attachAxis(axisX);
    s->attachAxis(axisY);

    series.append(s);
}

float LiveChart::getTime() const {
    return static_cast<float>(QDateTime::currentMSecsSinceEpoch() - start)/1000.f;
}

void LiveChart::append(const QString name, const float value) {
    if(ignoreSamples) {
        return;
    }

    QtCharts::QLineSeries *selectedSeries = nullptr;

    for(QtCharts::QLineSeries *existingSeries : series) {
        if(existingSeries->name()==name) {
            selectedSeries = existingSeries;
            break;
        }
    }

    if(!selectedSeries) {
        return;
    }

    selectedSeries->append(getTime(), value);
}

void LiveChart::resume() {
    timer->start();
    ignoreSamples = false;

    for(QtCharts::QLineSeries *s : series) {
        s->clear();
    }
}

void LiveChart::pause() {
    timer->stop();
    ignoreSamples = true;
}

}
