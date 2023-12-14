#include "livechart.h"
#include <QDateTime>
#include <QDebug>
#include <QChartView>
#include <QLegend>
#include <QFileDialog>

namespace widgets {

const qint64 LiveChart::start = QDateTime::currentMSecsSinceEpoch();
bool LiveChart::paused = false;
QVector<QtCharts::QLineSeries *> LiveChart::series;

LiveChart::LiveChart(const Config &config, QWidget *parent) : QWidget{parent} {
    setFixedSize(450, 300);

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
        if(paused) {
            return;
        }

        const float t = getTime();

        axisX->setRange(t-10, t);

        for(QtCharts::QLineSeries *s : series) {
            if(s->chart()==chart) {
                for(const QPointF &p : s->points()) {
                    if(p.x()<t-10) {
                        s->remove(p);
                    }
                }
            }
        }
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

float LiveChart::getTime() {
    return static_cast<float>(QDateTime::currentMSecsSinceEpoch() - start)/1000.f;
}

void LiveChart::append(const QString name, const float value) {
    for(QtCharts::QLineSeries *s : series) {
        if(s->name()==name && s->chart()==chart) {
            s->append(getTime(), value);
            return;
        }
    }
}

void LiveChart::resume() {
    paused = false;

    for(QtCharts::QLineSeries *s : series) {
        s->clear();
    }
}

void LiveChart::pause() {
    paused = true;
}

void LiveChart::save() {
    pause();

    QString basename = "livechart_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH:mm:ss");
    basename = QFileDialog::getSaveFileName(nullptr, "DialogTitle", basename).replace(" ", "_");

    QDir().mkdir(basename);

    for(QtCharts::QLineSeries *s : series) {
        const QString name =  s->chart()->title().replace(" ", "_") + "_" + s->name().replace(" ", "_");

        QFile file(basename + "/" + name + ".csv");

        if(!file.open(QFile::WriteOnly | QFile::Text)) {
            qDebug() << "error during file save for" << name;
            continue;
        }

        QTextStream output(&file);

        output << "time," << name << "\n";
        for(const QPointF &point : s->points()) {
            output << point.x() << "," << point.y() << "\n";
        }

        file.close();
    }
}

}
