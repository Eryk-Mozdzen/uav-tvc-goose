#include "window.h"
#include <QHBoxLayout>
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <iostream>

Window::Window(QWidget *parent) : QWidget(parent), J{0, 9} {

    offset[0] = 0.f;
    offset[1] = 0.f;
    offset[2] = 0.f;

    scale[0] = 1.f;
    scale[1] = 1.f;
    scale[2] = 1.f;

    QHBoxLayout *layout = new QHBoxLayout(this);

    raw = new Viewer(200, this);
    calibrated = new Viewer(200, this);

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
    const float x = (sample.x - offset[0])/scale[0];
    const float y = (sample.y - offset[1])/scale[1];
    const float z = (sample.z - offset[2])/scale[2];

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
    const int N = samples.size();

    J.append({s.x*s.x, s.y*s.y, s.z*s.z, s.x*s.y, s.x*s.z, s.y*s.z, s.x, s.y, s.z});

    Matrix K(N, 1);
    for(int i=0; i<N; i++) {
        K(i, 0) = 1.f;
    }

    const Matrix JT = J.transposition();
    const Matrix JTJ = JT*J;
    const Matrix ABC = JTJ.inverse()*JT*K;

    const float A = ABC(0, 0);
    const float B = ABC(1, 0);
    const float C = ABC(2, 0);
    const float G = ABC(6, 0);
    const float H = ABC(7, 0);
    const float I = ABC(8, 0);

    scale[0] = sqrt(1.f/A);
    scale[1] = sqrt(1.f/B);
    scale[2] = sqrt(1.f/C);

    offset[0] = -0.5f*G*scale[0]*scale[0];
    offset[1] = -0.5f*H*scale[1]*scale[1];
    offset[2] = -0.5f*I*scale[2]*scale[2];

    std::cout << ABC << std::endl;
    qDebug() << "offset:" << offset[0] << offset[1] << offset[2];
    qDebug() << "scale: " << scale[0] << scale[1] << scale[2];

    raw->clear();
    calibrated->clear();

    for(const Sample &sample : samples) {
        raw->add(sample);
        calibrated->add(get_calibrated(sample));
    }
}
