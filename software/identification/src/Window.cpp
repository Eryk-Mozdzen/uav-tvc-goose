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

        layout->addRow("Throttle start [%]", start_line);
        layout->addRow("Throttle stop [%]", stop_line);
        layout->addRow("Number of steps", steps_line);
        layout->addRow("Wait time [s]", wait_line);
        layout->addRow("Sample time [s]", sample_line);

        grid->addWidget(group, 2, 0);
    }

    {
        QGroupBox *group = new QGroupBox("Experiment");
        QGridLayout *layout  = new QGridLayout(group);

        group->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        load_label = new QLabel("--- g");

        QFont font;
        font.setWeight(QFont::Weight::Bold);
        font.setPointSize(20);
        load_label->setFont(font);
        load_label->setAlignment(Qt::AlignHCenter);

        QPushButton *start_button = new QPushButton("Start");
        QPushButton *stop_button = new QPushButton("Stop");
        QPushButton *save_button = new QPushButton("Save");
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
            setThrottle(start);
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

        grid->addWidget(group, 0, 1, 3, 1);
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
}

void Window::receive(const uint8_t id, const double time, const QByteArray &payload) {
    (void)time;

    if(id==MSG_ID_SENSOR) {
        const msg_frame_sensor_t *sensor = reinterpret_cast<const msg_frame_sensor_t *>(payload.data());

        if(sensor->valid.load) {
            const double w1 = static_cast<double>(avg_num)/static_cast<double>(avg_num + 1);
            const double w2 = 1./static_cast<double>(avg_num + 1);

            avg_load = w1*avg_load + w2*sensor->load.calib;
            avg_num++;

            load_label->setText(QString::asprintf("%5.3f kg", sensor->load.calib));
        }
    }
}

void Window::setThrottle(const int value) {
    const int constrained = (value>100) ? 100 : (value<0) ? 0 : value;

    msg_frame_manual_t manual;
    manual.motor = 10*constrained + 1000;

    transmit(MSG_ID_MANUAL, QByteArray(reinterpret_cast<const char *>(&manual), sizeof(manual)));
}
