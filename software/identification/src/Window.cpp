#include <fstream>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>
#include <QTimer>
#include <QIntValidator>
#include <QDoubleValidator>

#include "Window.h"

Window::Window(QWidget *parent) : QWidget{parent} {
    QGridLayout *grid = new QGridLayout(this);

    {
        QGroupBox *group = new QGroupBox("settings");
        QFormLayout *layout = new QFormLayout(group);

        group->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        start_line = new QLineEdit(QString::asprintf("%d", start));
        stop_line = new QLineEdit(QString::asprintf("%d", stop));
        steps_line = new QLineEdit(QString::asprintf("%d", steps));
        wait_line = new QLineEdit(QString::asprintf("%.1f", wait_time));
        sample_line = new QLineEdit(QString::asprintf("%.1f", sample_time));

        start_line->setValidator(new QIntValidator(0, 100));
        stop_line->setValidator(new QIntValidator(0, 100));
        steps_line->setValidator(new QIntValidator(1, 100));
        wait_line->setValidator(new QDoubleValidator(0, 10, 1));
        sample_line->setValidator(new QDoubleValidator(0, 10, 1));

        connect(start_line, &QLineEdit::returnPressed, [&]() {
            start = start_line->text().toDouble();
        });

        connect(stop_line, &QLineEdit::returnPressed, [&]() {
            stop = stop_line->text().toDouble();
        });

        connect(steps_line, &QLineEdit::returnPressed, [&]() {
            steps = steps_line->text().toDouble();
        });

        connect(wait_line, &QLineEdit::returnPressed, [&]() {
            wait_time = wait_line->text().toDouble();
        });

        connect(sample_line, &QLineEdit::returnPressed, [&]() {
            sample_time = sample_line->text().toDouble();
        });

        layout->addRow("throttle start [%]", start_line);
        layout->addRow("throttle stop [%]", stop_line);
        layout->addRow("number of steps", steps_line);
        layout->addRow("wait time [s]", wait_line);
        layout->addRow("sample time [s]", sample_line);

        grid->addWidget(group, 0, 0);
    }

    {
        QGroupBox *group = new QGroupBox("experiment");
        QGridLayout *layout  = new QGridLayout(group);

        group->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        load_label = new QLabel("--- g");

        QFont font;
        font.setWeight(QFont::Weight::Bold);
        font.setPointSize(20);
        load_label->setFont(font);
        load_label->setAlignment(Qt::AlignHCenter);

        QPushButton *start_button = new QPushButton("start");
        QPushButton *stop_button = new QPushButton("stop");
        QPushButton *save_button = new QPushButton("save");
        data_text = new QTextEdit();
        data_text->setReadOnly(true);

        connect(start_button, &QPushButton::clicked, [&]() {
            timer_step.stop();
            timer_zero.stop();
            timer_step.setSingleShot(true);
            timer_zero.setSingleShot(true);
            timer_step.setInterval(1000*(wait_time + sample_time));
            timer_zero.setInterval(1000*wait_time);

            step = 0;
            data_text->clear();
            data_text->append("throttle,load");

            timer_step.start();
            timer_zero.start();
        });

        connect(stop_button, &QPushButton::clicked, [&]() {
            setThrottle(0);
            timer_step.stop();
            timer_zero.stop();
        });

        connect(save_button, &QPushButton::clicked, [&]() {
            const QString filepath = QFileDialog::getSaveFileName(this, "Save CSV File", "", "CSV Files (*.csv);;All Files (*)");

            if(!filepath.isEmpty()) {
                std::ofstream file(filepath.toStdString());
                file << data_text->toPlainText().toStdString();
                file.close();
            }
        });

        layout->addWidget(load_label, 0, 0, 1, 2);
        layout->addWidget(start_button, 1, 0);
        layout->addWidget(stop_button, 1, 1);
        layout->addWidget(data_text, 2, 0, 1, 2);
        layout->addWidget(save_button, 3, 0, 1, 2);

        grid->addWidget(group, 0, 1);
    }

    connect(&timer_step, &QTimer::timeout, [&]() {
        const double s = (((double)stop) - ((double)start))/((double)steps);
        const double throttle = step*s + start;

        data_text->append(QString::asprintf("%.3f,%.4f", throttle/100, avg_load));

        if(step>=steps) {
            setThrottle(0);
            timer_step.stop();
            timer_zero.stop();
            return;
        }

        step++;
        setThrottle(throttle);

        timer_step.setSingleShot(true);
        timer_step.setInterval(1000*(wait_time + sample_time));
        timer_step.start();
    });

    connect(&timer_zero, &QTimer::timeout, [&]() {
        avg_num = 0;

        timer_zero.setSingleShot(true);
        timer_zero.setInterval(1000*(wait_time + sample_time));
        timer_zero.start();
    });

    // remove this
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [&]() {
        protocol_message_t message;
        message.id = PROTOCOL_ID_LOG;
        message.size = 0;
        transmit(message);
    });
    timer->start(100);
}

void Window::receive(const protocol_message_t &frame) {
    if(frame.id==PROTOCOL_ID_READINGS) {
        protocol_readings_t *readings = reinterpret_cast<protocol_readings_t *>(frame.payload);

        if(readings->valid.load) {
            const double w1 = static_cast<double>(avg_num)/static_cast<double>(avg_num + 1);
            const double w2 = 1./static_cast<double>(avg_num + 1);

            avg_load = w1*avg_load + w2*readings->calibrated.load;
            avg_num++;

            load_label->setText(QString::asprintf("%5.3f kg", readings->calibrated.load));
        }
    }
}

void Window::setThrottle(const int value) {
    const int constrained = (value>100) ? 100 : (value<0) ? 0 : value;

    protocol_control_t control;
    control.motor = 10*constrained + 1000;

    protocol_message_t message;
    message.payload =&control;
    message.size = sizeof(control);
    message.id = PROTOCOL_ID_CONTROL;

    transmit(message);
}
