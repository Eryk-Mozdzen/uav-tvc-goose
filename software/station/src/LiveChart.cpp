#include <cassert>

#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QLegend>
#include <QGraphicsLayout>
#include <QFont>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QDebug>

#include "LiveChart.h"

qint64 LiveChart::start = QDateTime::currentMSecsSinceEpoch();
bool LiveChart::paused = false;
QVector<QLineSeries *> LiveChart::series;

LiveChart::LiveChart(const Config &config, QWidget *parent) : QChartView{parent} {
    chart = new QChart();
    chart->setTitle(config.title);
    chart->legend()->hide();
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setContentsMargins(-20, -20, -20, -20);
    chart->setBackgroundRoundness(0);
    chart->setBackgroundBrush(Qt::transparent);

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);
    setMinimumSize(300, 200);

    axisX = new QValueAxis(this);
    axisX->setLabelFormat("%5.1f");
    axisX->setTitleFont(QFont());
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis(this);
    axisY->setTitleText(config.yLabel);
    axisY->setLabelFormat(config.yFormat);
    axisY->setRange(config.yMin, config.yMax);
    axisY->setTitleFont(QFont());
    chart->addAxis(axisY, Qt::AlignLeft);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        if(paused) {
            return;
        }

        const float t = getTime();

        axisX->setRange(t-10, t);

        for(QLineSeries *s : series) {
            if(s->chart()==chart) {
                while(s->count()>0) {
                    if(s->at(0).x()<t-10) {
                        s->remove(0);
                    } else {
                        break;
                    }
                }
            }
        }
    });
    timer->start(20);
}

void LiveChart::addSeries(const QString name, const QPen pen) {
    QLineSeries *s = new QLineSeries(this);

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
    bool found = false;

    for(QLineSeries *s : series) {
        if(s->name()==name && s->chart()==chart) {
            s->append(getTime(), value);
            found = true;
            break;
        }
    }

    assert(found);
}

void LiveChart::resume() {
    paused = false;

    for(QLineSeries *s : series) {
        s->clear();
    }

    LiveChart::start = QDateTime::currentMSecsSinceEpoch();
}

void LiveChart::pause() {
    paused = true;
}

void LiveChart::save() {
    pause();

    QString basename = "livechart_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH:mm:ss");

    basename = QFileDialog::getSaveFileName(nullptr, "DialogTitle", basename).replace(" ", "_");

    if(basename.isEmpty()) {
        return;
    }

    QDir().mkdir(basename);

    for(QLineSeries *s : series) {
        const QString name =  s->chart()->title().replace(" ", "_") + "_" + s->name().replace(" ", "_");

        QFile file(basename + "/" + name + ".csv");

        if(!file.open(QFile::WriteOnly | QFile::Text)) {
            qDebug() << "error during file save for" << name;
            continue;
        }

        QTextStream output(&file);

        output << "time,value\n";
        for(const QPointF &point : s->points()) {
            output << point.x() << "," << point.y() << "\n";
        }

        file.close();
    }
}
