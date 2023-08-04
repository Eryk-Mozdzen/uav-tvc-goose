#include "window.h"
#include <QHBoxLayout>
#include <QDebug>
#include <cmath>
#include <algorithm>

Window::Window(QWidget *parent) : QWidget(parent) {

    scale[0] = 1.f;
    scale[1] = 1.f;
    scale[2] = 1.f;

    QHBoxLayout *layout = new QHBoxLayout(this);

    raw = new Viewer(200, this);
    calibrated = new Viewer(100, this);

    layout->addWidget(raw);
    layout->addWidget(calibrated);

    connect(&usb, &USB::receive, this, &Window::callback);
}

bool Window::is_far_enough(const Sample &test) const {
    constexpr float min_dist2 = min_dist*min_dist;

    for(const Sample &sample : samples) {
        const float dx = test.x - sample.x;
        const float dy = test.y - sample.y;
        const float dz = test.z - sample.z;

        const float dist2 = dx*dx + dy*dy + dz*dz;

        if(dist2<min_dist2) {
            return false;
        }
    }

    return true;
}

Window::Sample Window::get_calibrated(const Sample &sample) const {
    const float x = scale[0]*(sample.x - offset[0]);
    const float y = scale[1]*(sample.y - offset[1]);
    const float z = scale[2]*(sample.z - offset[2]);

    return {x, y, z};
}

void Window::callback(Transfer::FrameRX frame) {
    if(frame.id!=Transfer::ID::SENSOR_MAGNETIC_FIELD) {
        return;
    }

    Sample s;
    if(!frame.getPayload(s)) {
        return;
    }

    if(!is_far_enough(s)) {
        return;
    }

    samples.push_back(s);

    raw->add(s);

    const float num = samples.size();
    offset[0] = std::accumulate(samples.begin(), samples.end(), 0.0f, [](float sum, const Sample& s) {return sum + s.x;})/num;
    offset[1] = std::accumulate(samples.begin(), samples.end(), 0.0f, [](float sum, const Sample& s) {return sum + s.y;})/num;
    offset[2] = std::accumulate(samples.begin(), samples.end(), 0.0f, [](float sum, const Sample& s) {return sum + s.z;})/num;

    scale[0] = num/std::accumulate(samples.begin(), samples.end(), 0.0f, [this](float sum, const Sample& s) {return sum + fabs(s.x - offset[0]);});
    scale[1] = num/std::accumulate(samples.begin(), samples.end(), 0.0f, [this](float sum, const Sample& s) {return sum + fabs(s.y - offset[1]);});
    scale[2] = num/std::accumulate(samples.begin(), samples.end(), 0.0f, [this](float sum, const Sample& s) {return sum + fabs(s.z - offset[2]);});

    //qDebug() << offset[0] << offset[1] << offset[2];

    calibrated->clear();

    for(const Sample &sample : samples) {
        calibrated->add(get_calibrated(sample));
    }
}
