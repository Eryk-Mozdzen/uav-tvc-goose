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
#include <QThread>

#include "common/protocol/msg.h"
#include "common/qt/Serial.h"
#include "common/qt/Network.h"
#include "common/qt/InterfaceWidget.h"
#include "Window.h"

Window::Window(QWidget *parent) : QWidget{parent} {
    QGridLayout *grid = new QGridLayout(this);

    {
        common::Serial *serial = new common::Serial();
	    common::Network *network = new common::Network();

        connect(serial, &common::Serial::receive, this, &Window::receive);
        connect(network, &common::Network::receive, this, &Window::receive);
        connect(this, &Window::transmit, serial, &common::Serial::transmit);
        connect(this, &Window::transmit, network, &common::Network::transmit);

        common::InterfaceWidget *serialInterface = new common::InterfaceWidget("Serial interface", this);
        common::InterfaceWidget *networkInterface = new common::InterfaceWidget("Network interface", this);

        connect(serial, &common::Serial::stats, serialInterface, &common::InterfaceWidget::stats);
        connect(serial, &common::Serial::status, serialInterface, &common::InterfaceWidget::status);
        connect(serial, &common::Serial::scanFinished, serialInterface, &common::InterfaceWidget::scanFinished);
        connect(serialInterface, &common::InterfaceWidget::scan, serial, &common::Serial::scanPorts);
        connect(serialInterface, &common::InterfaceWidget::change, serial, &common::Serial::changePort);

        connect(network, &common::Network::stats, networkInterface, &common::InterfaceWidget::stats);
        connect(network, &common::Network::status, networkInterface, &common::InterfaceWidget::status);
        connect(network, &common::Network::scanFinished, networkInterface, &common::InterfaceWidget::scanFinished);
        connect(networkInterface, &common::InterfaceWidget::scan, network, &common::Network::scanHosts);
        connect(networkInterface, &common::InterfaceWidget::change, network, &common::Network::changeHost);

        QThread *serialThread = new QThread(this);
        QThread *networkThread = new QThread(this);

        serial->moveToThread(serialThread);
        connect(serialThread, &QThread::started, serial, &common::Serial::start);
        connect(serialThread, &QThread::finished, serial, &common::Serial::deleteLater);
        connect(serialThread, &QThread::finished, serialThread, &QThread::deleteLater);
        connect(this, &QObject::destroyed, serialThread, &QThread::quit);

        network->moveToThread(networkThread);
        connect(networkThread, &QThread::started, network, &common::Network::start);
        connect(networkThread, &QThread::finished, network, &common::Network::deleteLater);
        connect(networkThread, &QThread::finished, networkThread, &QThread::deleteLater);
        connect(this, &QObject::destroyed, networkThread, &QThread::quit);

        serialThread->start();
        networkThread->start();

        serialInterface->forceScan();
        networkInterface->forceScan();

        grid->addWidget(serialInterface, 0, 0);
        grid->addWidget(networkInterface, 1, 0);
    }

    {
        QGroupBox *group = new QGroupBox("Settings");
        QFormLayout *layout = new QFormLayout(group);

        group->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        start_line = new QLineEdit(QString::asprintf("%d", start));
        stop_line = new QLineEdit(QString::asprintf("%d", stop));
        steps_line = new QLineEdit(QString::asprintf("%d", steps));
        wait_line = new QLineEdit(QString::asprintf("%.1f", wait_time));
        sample_line = new QLineEdit(QString::asprintf("%.1f", sample_time));

        QValidator *start_validator = new QIntValidator(0, 100, this);
        QValidator *stop_validator = new QIntValidator(0, 100, this);
        QValidator *steps_validator = new QIntValidator(1, 100, this);
        QValidator *wait_validator = new QDoubleValidator(0, 10, 1, this);
        QValidator *sample_validator = new QDoubleValidator(0, 10, 1, this);

        start_validator->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        stop_validator->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        steps_validator->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        wait_validator->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        sample_validator->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        start_line->setValidator(start_validator);
        stop_line->setValidator(stop_validator);
        steps_line->setValidator(steps_validator);
        wait_line->setValidator(wait_validator);
        sample_line->setValidator(sample_validator);

        connect(start_line, &QLineEdit::returnPressed, [this]() {
            start = start_line->text().toDouble();
        });

        connect(stop_line, &QLineEdit::returnPressed, [this]() {
            stop = stop_line->text().toDouble();
        });

        connect(steps_line, &QLineEdit::returnPressed, [this]() {
            steps = steps_line->text().toDouble();
        });

        connect(wait_line, &QLineEdit::returnPressed, [this]() {
            wait_time = wait_line->text().toDouble();
        });

        connect(sample_line, &QLineEdit::returnPressed, [this]() {
            sample_time = sample_line->text().toDouble();
        });

        layout->addRow("Throttle start [%]", start_line);
        layout->addRow("Throttle stop [%]", stop_line);
        layout->addRow("Number of steps", steps_line);
        layout->addRow("Wait time [s]", wait_line);
        layout->addRow("Sample time [s]", sample_line);

        grid->addWidget(group, 2, 0);
    }

    {
        QGroupBox *group = new QGroupBox("Experiment", this);
        QGridLayout *layout  = new QGridLayout(group);

        group->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        QLabel *label1 = new QLabel("thrust:", this);
        QLabel *label2 = new QLabel("velocity:", this);
        QLabel *label3 = new QLabel("current:", this);
        QLabel *label4 = new QLabel("voltage:", this);

        label[0] = new QLabel("--- g", this);
        label[1] = new QLabel("--- rad/s", this);
        label[2] = new QLabel("--- A", this);
        label[3] = new QLabel("--- V", this);

        label1->setAlignment(Qt::AlignRight);
        label2->setAlignment(Qt::AlignRight);
        label3->setAlignment(Qt::AlignRight);
        label4->setAlignment(Qt::AlignRight);

        label[0]->setAlignment(Qt::AlignLeft);
        label[1]->setAlignment(Qt::AlignLeft);
        label[2]->setAlignment(Qt::AlignLeft);
        label[3]->setAlignment(Qt::AlignLeft);

        layout->addWidget(label1, 0, 0);
        layout->addWidget(label2, 1, 0);
        layout->addWidget(label3, 2, 0);
        layout->addWidget(label4, 3, 0);

        layout->addWidget(label[0], 0, 1);
        layout->addWidget(label[1], 1, 1);
        layout->addWidget(label[2], 2, 1);
        layout->addWidget(label[3], 3, 1);

        QPushButton *start_button = new QPushButton("Start", this);
        QPushButton *stop_button = new QPushButton("Stop", this);
        QPushButton *save_button = new QPushButton("Save", this);
        data_text = new QTextEdit();
        data_text->setReadOnly(true);
        data_text->setMinimumWidth(300);

        connect(start_button, &QPushButton::clicked, [this]() {
            timer_step.stop();
            timer_zero.stop();
            timer_step.setSingleShot(true);
            timer_zero.setSingleShot(true);
            timer_step.setInterval(1000*(wait_time + sample_time));
            timer_zero.setInterval(1000*wait_time);

            step = 0;
            setThrottle(start);
            data_text->clear();
            data_text->append("throttle,load,velocity,current,voltage");

            timer_step.start();
            timer_zero.start();
        });

        connect(stop_button, &QPushButton::clicked, [this]() {
            setThrottle(0);
            timer_step.stop();
            timer_zero.stop();
        });

        connect(save_button, &QPushButton::clicked, [this]() {
            const QString filepath = QFileDialog::getSaveFileName(this, "Save CSV File", "", "CSV Files (*.csv);;All Files (*)");

            if(!filepath.isEmpty()) {
                std::ofstream file(filepath.toStdString());
                file << data_text->toPlainText().toStdString();
                file.close();
            }
        });

        layout->addWidget(start_button, 4, 0);
        layout->addWidget(stop_button, 4, 1);
        layout->addWidget(data_text, 5, 0, 1, 2);
        layout->addWidget(save_button, 6, 0, 1, 2);

        grid->addWidget(group, 0, 1, 3, 1);
    }

    connect(&timer_step, &QTimer::timeout, [this]() {
        const double s = (((double)stop) - ((double)start))/((double)steps);
        const double throttle = step*s + start;

        data_text->append(QString::asprintf("%.3f,%.4f,%.0f,%.2f,%.2f",
            throttle/100,
            load.get(),
            velocity.get(),
            current.get(),
            voltage.get()
        ));

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

    connect(&timer_zero, &QTimer::timeout, [this]() {
        load.reset();
        velocity.reset();
        current.reset();
        voltage.reset();

        timer_zero.setSingleShot(true);
        timer_zero.setInterval(1000*(wait_time + sample_time));
        timer_zero.start();
    });
}

void Window::receive(const uint8_t id, const double time, const QByteArray &payload) {
    (void)time;

    if(id==MSG_ID_SENSOR) {
        const msg_frame_sensor_t *sensor = reinterpret_cast<const msg_frame_sensor_t *>(payload.data());

        if(sensor->valid.load) {
            load.append(sensor->load.calib);

            label[0]->setText(QString::asprintf("%5.3f kg", load.get()));
        }

        if(sensor->valid.tachometer) {
            velocity.append(sensor->tachometer);

            label[1]->setText(QString::asprintf("%4.0f rad/s", velocity.get()));
        }

        if(sensor->valid.power) {
            voltage.append(sensor->power[0]);
            current.append(sensor->power[1]);

            label[2]->setText(QString::asprintf("%5.2f A", current.get()));
            label[3]->setText(QString::asprintf("%5.2f V", voltage.get()));
        }
    }
}

void Window::setThrottle(const int value) {
    const int constrained = (value>100) ? 100 : (value<0) ? 0 : value;

    msg_frame_manual_t manual;
    manual.motor = 10*constrained + 1000;

    transmit(MSG_ID_MANUAL, QByteArray(reinterpret_cast<const char *>(&manual), sizeof(manual)));
}
