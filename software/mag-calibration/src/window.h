#pragma once

#include "viewer.h"
#include "usb.h"

class Window : public QWidget {
    Q_OBJECT

    using Sample = Viewer::Sample;

    static constexpr float min_dist = 0.25f;

    Viewer *raw;
    Viewer *calibrated;
    USB usb;

    float offset[3];
    float scale[3];
    QVector<Sample> samples;

    bool is_far_enough(const Sample &test) const;
    Sample get_calibrated(const Sample &sample) const;

private slots:
    void callback(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
