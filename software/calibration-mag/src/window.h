#pragma once

#include "viewer.h"
#include "usb.h"
#include "matrix.h"
#include "utils.h"

class Window : public QWidget {
    Q_OBJECT

    static constexpr float min_dist = 0.05f;

    Viewer *raw;
    Viewer *calibrated;
    USB usb;

    QVector<Sample> samples;
    Matrix J;
    Matrix K;

    bool is_far_enough(const Sample &test) const;

private slots:
    void callback(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
