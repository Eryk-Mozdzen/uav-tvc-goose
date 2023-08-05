#include "window.h"
#include <QHBoxLayout>

Window::Window(QWidget *parent) : QWidget(parent) {

    axis[0] = new Axis('X');
    axis[1] = new Axis('Y');
    axis[2] = new Axis('Z');

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(axis[0]);
    layout->addWidget(axis[1]);
    layout->addWidget(axis[2]);

    connect(&usb, &USB::receive, this, &Window::callback);
}

void Window::callback(Transfer::FrameRX frame) {
    if(frame.id!=Transfer::ID::SENSOR_ACCELERATION) {
        return;
    }

    struct { float x, y, z; } acc;
    frame.getPayload(acc);

    axis[0]->set(acc.x);
    axis[1]->set(acc.y);
    axis[2]->set(acc.z);
}
