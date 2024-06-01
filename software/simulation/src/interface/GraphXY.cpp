#include <QChartView>
#include <QLegend>
#include <QTimer>

#include "GraphXY.h"

GraphXY::GraphXY(const QString title, const QString format, const float range) : QMainWindow{nullptr} {
    resize(600, 600);
    setWindowTitle(title);

    chart = new QtCharts::QChart();
    chart->legend()->hide();

    axisX = new QtCharts::QValueAxis(this);
    axisX->setTitleText("X");
    axisX->setLabelFormat(format);
    axisX->setRange(-range, range);
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QtCharts::QValueAxis(this);
    axisY->setTitleText("Y");
    axisY->setLabelFormat(format);
    axisY->setRange(-range, range);
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
                const double x = point(0);
                const double y = point(1);

                s.series->append(x, y);
                s.queue->pop_front();
            }

            while(s.series->count()>samples) {
                s.series->remove(0);
            }
        }
    });
    timer->start(1000*period);

    DeclarePeriodicPublishEvent(period, 0, &GraphXY::update);
}

void GraphXY::AddSeries(const QString name, const Eigen::MatrixX<double> selector, const QColor color, const Qt::PenStyle style, const int width) {
    Series s;
    s.series = new QtCharts::QLineSeries();
    s.queue = std::make_unique<SharedQueue<Eigen::Vector2d>>();
    chart->addSeries(s.series);
    s.series->setPen(QPen(color, width, style));
    s.series->attachAxis(axisX);
    s.series->attachAxis(axisY);

    s.selector = selector;
    s.port = DeclareVectorInputPort(name.toStdString(), selector.cols()).get_index();

    series.push_back(std::move(s));
}

drake::systems::EventStatus GraphXY::update(const drake::systems::Context<double> &context) const {

    for(const Series &s : series) {
        const Eigen::VectorXd input = EvalVectorInput(context, s.port)->CopyToVector();
        const Eigen::Vector2d point = s.selector*input;

        s.queue->push_back(point);
    }

    return drake::systems::EventStatus::Succeeded();
}
