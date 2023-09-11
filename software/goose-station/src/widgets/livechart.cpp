#include "livechart.h"
#include <QTimer>
#include <QDateTime>
#include <QValueAxis>
#include <QDebug>

namespace widgets {

LiveChart::LiveChart(const float _minY, const float _maxY, QWidget *parent) : QWidget{parent},
        start{QDateTime::currentMSecsSinceEpoch()},
        minY{_minY},
        maxY{_maxY} {
    setFixedSize(500, 320);

    chart = new QtCharts::QChart();
    chart->createDefaultAxes();
    chart->legend()->hide();

    view = new QtCharts::QChartView(chart, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setGeometry(0, 0, width(), height());

    QTimer *timer = new QTimer(this);
    timer->setInterval(20);
    timer->start();

    connect(timer, &QTimer::timeout, [this]() {
        const float t = getTime();

        QtCharts::QValueAxis *axisX = new QtCharts::QValueAxis;
        axisX->setRange(t-10, t);
        //axisX->setTickCount(1000);
        //axisX->setLabelFormat("%.2f");
        //chart->setAxisX(axisX);

        QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis;
        axisY->setRange(minY, maxY);
        //axisY->setTickCount(0.1);
        //axisX->setLabelFormat("%.2f");
        //chart->setAxisY(axisY);

        for(QtCharts::QLineSeries *s : series) {
            chart->setAxisX(axisX, s);
            chart->setAxisY(axisY, s);
        }
    });
}

void LiveChart::addSeries(const QString name, const QPen pen) {
    QtCharts::QLineSeries *s = new QtCharts::QLineSeries();
    s->setPen(pen);
    s->setName(name);

    chart->addSeries(s);
    series.append(s);
}

float LiveChart::getTime() const {
    return static_cast<float>(QDateTime::currentMSecsSinceEpoch() - start)/1000.f;
}

void LiveChart::append(const QString name, const float value) {
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

}
