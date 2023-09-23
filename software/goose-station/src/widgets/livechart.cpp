#include "livechart.h"
#include <QDateTime>
#include <QDebug>
#include <QChartView>
#include <QLegend>

namespace widgets {

LiveChart::LiveChart(const Config &config, QWidget *parent) : QWidget{parent},
        start{QDateTime::currentMSecsSinceEpoch()},
        paused{false} {
    setFixedSize(450, 320);

    chart = new QtCharts::QChart();
    chart->setTitle(config.title);

    chart->legend()->hide();
    //chart->legend()->setAlignment(Qt::AlignLeft);
    //chart->legend()->setMarkerShape(QtCharts::QLegend::MarkerShapeFromSeries);

    QtCharts::QChartView *view = new QtCharts::QChartView(chart, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setGeometry(0, 0, width(), height());

    axisX = new QtCharts::QValueAxis(this);
    axisX->setTitleText("time [s]");
    axisX->setLabelFormat("%5.1f");
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QtCharts::QValueAxis(this);
    axisY->setTitleText(config.yLabel);
    axisY->setLabelFormat(config.yFormat);
    axisY->setRange(config.yMin, config.yMax);
    chart->addAxis(axisY, Qt::AlignLeft);

    timer = new QTimer(this);
    timer->setInterval(20);
    timer->start();

    connect(timer, &QTimer::timeout, [this]() {
        const float t = getTime();

        axisX->setRange(t-10, t);
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
    if(paused) {
        return;
    }

    for(QtCharts::QLineSeries *s : series) {
        if(s->name()==name) {
            s->append(getTime(), value);
            return;
        }
    }
}

void LiveChart::resume() {
    timer->start();
    paused = false;

    for(QtCharts::QLineSeries *s : series) {
        s->clear();
    }
}

void LiveChart::pause() {
    timer->stop();
    paused = true;
}

}
