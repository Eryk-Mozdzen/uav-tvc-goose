#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QTimer>

#include "protocol/protocol.h"
#include "protocol/protocol_data.h"

class Window : public QWidget {
    Q_OBJECT

    double raw;
    double load;
    double avg_load;
    double avg_num = 0;
    double throttle;

    double min = 8500000;
    double max = 9500000;
    double start = 0;
    double stop = 1;
    double step = 0.05;
    double wait_time = 1;
    double sample_time = 3;

    QLineEdit *min_line;
    QLineEdit *max_line;
    QLabel *load_label;
    QLabel *raw_label;
    QTextEdit *data_text;
    QTimer timer_step;
    QTimer timer_zero;

    void setThrottle(const double value);

signals:
    void transmit(const protocol_message_t &frame);

public slots:
    void receive(const protocol_message_t &frame);

public:
    Window(QWidget *parent = nullptr);
};
