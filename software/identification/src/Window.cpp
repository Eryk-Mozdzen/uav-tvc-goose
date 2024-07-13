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
#include <QDoubleValidator>

#include "Window.h"

Window::Window(QWidget *parent) : QWidget{parent} {
    QGridLayout *grid = new QGridLayout(this);

    {
        QGroupBox *group = new QGroupBox("load cell calibration");
        QGridLayout *layout  = new QGridLayout(group);

        min_line = new QLineEdit(QString::asprintf("%.0f", min));
        max_line = new QLineEdit(QString::asprintf("%.0f", max));
        min_line->setReadOnly(true);
        max_line->setReadOnly(true);

        QPushButton *min_button = new QPushButton("update");
        QPushButton *max_button = new QPushButton("update");

        connect(min_button, &QPushButton::clicked, [&]() {
            min_line->setText(QString::asprintf("%.0f", raw));
            min = raw;
        });

        connect(max_button, &QPushButton::clicked, [&]() {
            max_line->setText(QString::asprintf("%.0f", raw));
            max = raw;
        });

        layout->addWidget(new QLabel("reference (0 kg)"), 0, 0);
        layout->addWidget(new QLabel("reference (1 kg)"), 1, 0);
        layout->addWidget(min_line, 0, 1);
        layout->addWidget(max_line, 1, 1);
        layout->addWidget(min_button, 0, 2);
        layout->addWidget(max_button, 1, 2);

        grid->addWidget(group, 0, 0);
    }

    {
        QGroupBox *group = new QGroupBox("experiment settings");
        QFormLayout *formLayout = new QFormLayout(group);

        start_line = new QLineEdit(QString::asprintf("%.0f", 100*start));
        stop_line = new QLineEdit(QString::asprintf("%.0f", 100*stop));
        step_line = new QLineEdit(QString::asprintf("%.0f", 100*step));
        wait_line = new QLineEdit(QString::asprintf("%.0f", wait_time));
        sample_line = new QLineEdit(QString::asprintf("%.0f", sample_time));

        start_line->setValidator(new QDoubleValidator(0, 100, 0));
        stop_line->setValidator(new QDoubleValidator(0, 100, 0));
        step_line->setValidator(new QDoubleValidator(0, 100, 0));
        wait_line->setValidator(new QDoubleValidator(0, 10, 1));
        sample_line->setValidator(new QDoubleValidator(0, 10, 1));

        connect(start_line, &QLineEdit::returnPressed, [&]() {
            start = start_line->text().toDouble();
        });

        connect(stop_line, &QLineEdit::returnPressed, [&]() {
            stop = stop_line->text().toDouble();
        });

        connect(step_line, &QLineEdit::returnPressed, [&]() {
            step = step_line->text().toDouble();
        });

        connect(wait_line, &QLineEdit::returnPressed, [&]() {
            wait_time = wait_line->text().toDouble();
        });

        connect(sample_line, &QLineEdit::returnPressed, [&]() {
            sample_time = sample_line->text().toDouble();
        });

        formLayout->addRow(new QLabel("throttle start [%]"), start_line);
        formLayout->addRow(new QLabel("throttle stop [%]"), stop_line);
        formLayout->addRow(new QLabel("throttle step [%]"), step_line);
        formLayout->addRow(new QLabel("wait time [s]"), wait_line);
        formLayout->addRow(new QLabel("sample time [s]"), sample_line);

        grid->addWidget(group, 1, 0);
    }

    {
        QGroupBox *group = new QGroupBox("experiment");
        QGridLayout *layout  = new QGridLayout(group);

        load_label = new QLabel("--- g");
        raw_label = new QLabel("---");

        QFont font;
        font.setWeight(QFont::Weight::Bold);
        font.setPointSize(20);
        load_label->setFont(font);
        load_label->setAlignment(Qt::AlignHCenter);
        raw_label->setAlignment(Qt::AlignHCenter);

        QPushButton *start_button = new QPushButton("start");
        QPushButton *stop_button = new QPushButton("stop");
        QPushButton *save_button = new QPushButton("save");
        data_text = new QTextEdit();

        connect(start_button, &QPushButton::clicked, [&]() {
            timer_step.stop();
            timer_zero.stop();
            timer_step.setSingleShot(true);
            timer_zero.setSingleShot(true);
            timer_step.setInterval(1000*(wait_time + sample_time));
            timer_zero.setInterval(1000*wait_time);

            throttle = start;
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
        layout->addWidget(raw_label, 1, 0, 1, 2);
        layout->addWidget(start_button, 2, 0);
        layout->addWidget(stop_button, 2, 1);
        layout->addWidget(data_text, 3, 0, 1, 2);
        layout->addWidget(save_button, 4, 0, 1, 2);

        grid->addWidget(group, 0, 1, 2, 1);
    }

    connect(&timer_step, &QTimer::timeout, [&]() {
        data_text->append(QString::asprintf("%.2f,%f", throttle, avg_load));

        if(throttle>=stop) {
            setThrottle(0);
            timer_step.stop();
            timer_zero.stop();
            return;
        }

        throttle +=step;
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
}

void Window::receive(const protocol_message_t &frame) {
    if(frame.id==PROTOCOL_ID_READINGS) {
        protocol_readings_t *readings = reinterpret_cast<protocol_readings_t *>(frame.payload);

        if(readings->valid.load) {
            raw = readings->load;
            load = (raw - min)/(max - min);

            avg_load = (avg_num/(avg_num + 1))*avg_load + (1/(avg_num + 1))*load;
            avg_num++;

            load_label->setText(QString::asprintf("%5.3f kg", load));
            raw_label->setText(QString::asprintf("%.0f", raw));
        }
    }
}

void Window::setThrottle(const double value) {
    const double constrained = (value>1) ? 1 : (value<0) ? 0 : value;

    protocol_control_t control;
    control.motor = 1000*constrained + 1000;

    protocol_message_t message;
    message.payload =&control;
    message.size = sizeof(control);
    message.id = PROTOCOL_ID_CONTROL;

    transmit(message);
}
