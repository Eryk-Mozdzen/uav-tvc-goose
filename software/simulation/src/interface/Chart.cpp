#include <QChartView>
#include <QLegend>
#include <QTimer>

#include "Chart.h"

Chart::Chart(const QString title, const QString yLabel, const QString yFormat, const float yMin, const float yMax) : QMainWindow{nullptr} {
    resize(600, 400);
    setWindowTitle(title);

    chart = new QtCharts::QChart();
    chart->legend()->hide();

    axisX = new QtCharts::QValueAxis(this);
    axisX->setTitleText("time [s]");
    axisX->setLabelFormat("%5.1f");
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QtCharts::QValueAxis(this);
    axisY->setTitleText(yLabel);
    axisY->setLabelFormat(yFormat);
    axisY->setRange(yMin, yMax);
    chart->addAxis(axisY, Qt::AlignLeft);

    QtCharts::QChartView *view = new QtCharts::QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(view);

    show();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        for(Series &s : series) {
            while(s.queue->size()>0) {
                const Eigen::Vector2d point = s.queue->front();
                const double time = point(0);
                const double value = point(1);

                axisX->setRange(time - horizion, time);
                s.series->append(time, value);
                s.queue->pop_front();
            }

            while(s.series->count()>samples) {
                s.series->remove(0);
            }
        }
    });
    timer->start(1000*period);

    DeclarePeriodicPublishEvent(period, 0, &Chart::update);
}

void Chart::AddSeries(const QString name, const Eigen::VectorXd selector, const QColor color, const Qt::PenStyle style, const int width) {
    Series s;
    s.series = new QtCharts::QLineSeries();
    s.queue = std::make_unique<SharedQueue<Eigen::Vector2d>>();
    chart->addSeries(s.series);
    s.series->setPen(QPen(color, width, style));
    s.series->attachAxis(axisX);
    s.series->attachAxis(axisY);

    s.selector = selector;
    s.port = DeclareVectorInputPort(name.toStdString(), selector.size()).get_index();

    series.push_back(std::move(s));
}

void Chart::AddSeries(const QString name, const Eigen::MatrixXd selector) {
    const int num = selector.rows();
    const drake::systems::InputPortIndex port = DeclareVectorInputPort(name.toStdString(), selector.cols()).get_index();

    const std::vector<QColor> colors = {
        Qt::red,
        Qt::green,
        Qt::blue,
        Qt::magenta,
        Qt::cyan
    };

    for(int i=0; i<num; i++) {
        Series s;
        s.series = new QtCharts::QLineSeries(this);
        s.queue = std::make_unique<SharedQueue<Eigen::Vector2d>>();
        chart->addSeries(s.series);
        s.series->setPen(QPen(colors.at(i), 2));
        s.series->attachAxis(axisX);
        s.series->attachAxis(axisY);

        s.selector = selector.row(i);
        s.port = port;

        series.push_back(std::move(s));
    }
}

drake::systems::EventStatus Chart::update(const drake::systems::Context<double> &context) const {
    const double time = context.get_time();

    for(const Series &s : series) {
        const Eigen::VectorXd &input = EvalVectorInput(context, s.port)->CopyToVector();
        const double value = s.selector.dot(input);

        s.queue->push_back(Eigen::Vector2d({time, value}));
    }

    return drake::systems::EventStatus::Succeeded();
}
