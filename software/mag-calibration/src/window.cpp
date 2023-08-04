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

    while(true) {
        const float N = samples.size();

        offset[0] = std::accumulate(samples.begin(), samples.end(), 0.0f, [](float sum, const Sample& s) {return sum + s.x;})/N;
        offset[1] = std::accumulate(samples.begin(), samples.end(), 0.0f, [](float sum, const Sample& s) {return sum + s.y;})/N;
        offset[2] = std::accumulate(samples.begin(), samples.end(), 0.0f, [](float sum, const Sample& s) {return sum + s.z;})/N;

        scale[0] = N/std::accumulate(samples.begin(), samples.end(), 0.0f, [this](float sum, const Sample& s) {return sum + fabs(s.x - offset[0]);});
        scale[1] = N/std::accumulate(samples.begin(), samples.end(), 0.0f, [this](float sum, const Sample& s) {return sum + fabs(s.y - offset[1]);});
        scale[2] = N/std::accumulate(samples.begin(), samples.end(), 0.0f, [this](float sum, const Sample& s) {return sum + fabs(s.z - offset[2]);});

        const float radius_mean = std::accumulate(samples.begin(), samples.end(), 0.0f,
            [this](float sum, const Sample& s) {
                const Sample calib = get_calibrated(s);
                return sum + sqrt(calib.x*calib.x + calib.y*calib.y + calib.z*calib.z);
            }
        )/N;

        const float radius_variance = std::accumulate(samples.begin(), samples.end(), 0.0f,
            [this, radius_mean](float sum, const Sample& s) {
                const Sample calib = get_calibrated(s);
                const float radius = sqrt(calib.x*calib.x + calib.y*calib.y + calib.z*calib.z);
                return sum + (radius - radius_mean)*(radius - radius_mean);
            }
        )/N;

        const float radius_sigma = sqrt(radius_variance);

        QVector<Sample> pass;
        bool more = false;
        for(const Sample &sample : samples) {
            const Sample calib = get_calibrated(sample);
            const float radius = sqrt(calib.x*calib.x + calib.y*calib.y + calib.z*calib.z);

            if(radius>(radius_mean+3*radius_sigma) || radius<(radius_mean-3*radius_sigma)) {
                more = true;
                continue;
            }

            pass.push_back(sample);
        }

        samples = pass;

        if(!more) {
            break;
        }
    }

    qDebug() << "offset:" << offset[0] << offset[1] << offset[2];
    qDebug() << "scale: " << scale[0] << scale[1] << scale[2];

    raw->clear();
    calibrated->clear();

    for(const Sample &sample : samples) {
        raw->add(sample);
        calibrated->add(get_calibrated(sample));
    }
}
