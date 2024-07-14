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

    double avg_load;
    int avg_num = 0;
    int step;

    int start = 0;
    int stop = 100;
    int steps = 20;
    double wait_time = 1;
    double sample_time = 3;

    QLabel *load_label;
    QTextEdit *data_text;
    QTimer timer_step;
    QTimer timer_zero;
    QLineEdit *start_line;
    QLineEdit *stop_line;
    QLineEdit *steps_line;
    QLineEdit *wait_line;
    QLineEdit *sample_line;

    void setThrottle(const int value);

signals:
    void transmit(const protocol_message_t &frame);

public slots:
    void receive(const protocol_message_t &frame);

public:
    Window(QWidget *parent = nullptr);
};
