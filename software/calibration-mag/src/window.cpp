#include "window.h"
#include <QHBoxLayout>
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

Window::Window(QWidget *parent) : QWidget(parent), J{0, 6}, K{0, 1} {
    raw = new Viewer(samples, this);
    calibrated = new Viewer(samples, this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
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

    J.append({s.x*s.x, s.y*s.y, s.z*s.z, s.x, s.y, s.z});
    K.append({1.f});

    const Matrix JT = J.transposition();
    const Matrix JTJ = JT*J;
    const Matrix ABC = JTJ.inverse()*JT*K;

    const float A = ABC(0, 0);
    const float B = ABC(1, 0);
    const float C = ABC(2, 0);
    const float G = ABC(3, 0);
    const float H = ABC(4, 0);
    const float I = ABC(5, 0);

    Params params;

    params.scale[0] = sqrt(1.f/A);
    params.scale[1] = sqrt(1.f/B);
    params.scale[2] = sqrt(1.f/C);

    params.offset[0] = -0.5f*G*params.scale[0]*params.scale[0];
    params.offset[1] = -0.5f*H*params.scale[1]*params.scale[1];
    params.offset[2] = -0.5f*I*params.scale[2]*params.scale[2];

    std::cout << "samples: " << std::setw(6) << std::noshowpos << samples.size() << " | ";
    std::cout << std::setprecision(4) << std::fixed << std::showpos;
    std::cout << "offset: " << params.offset[0] << " " << params.offset[1] << " " << params.offset[2] << " | ";
    std::cout << "scale: "  << params.scale[0]  << " " << params.scale[1]  << " " << params.scale[2]  << std::endl;

    raw->update();
    calibrated->set(params);
}
