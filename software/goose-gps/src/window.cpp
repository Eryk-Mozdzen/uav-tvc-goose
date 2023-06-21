#include "window.h"
#include <QHBoxLayout>
#include <QtCore>

Window::Window() {
    resize(640, 480);

    map = new InteractiveMap(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(map);
    layout->setMargin(0);

    connect(&timer, &QTimer::timeout, this, &Window::timerCallback);
    timer.setInterval(100);
    timer.start();
}

void Window::timerCallback() {
    const double lat = 51.103525733902586 + QRandomGenerator::global()->generateDouble()*0.0001 - 0.00005;
    const double lng = 17.085345490751223 + QRandomGenerator::global()->generateDouble()*0.0001 - 0.00005;

    map->mark(lat, lng);
}
