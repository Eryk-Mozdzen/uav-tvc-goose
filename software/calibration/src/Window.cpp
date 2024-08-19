#include <sstream>
#include <iomanip>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QThread>

#include "common/protocol/msg.h"
#include "common/qt/Serial.h"
#include "common/qt/Network.h"
#include "common/qt/InterfaceWidget.h"
#include "Window.h"
#include "Magnetometer.h"
#include "Accelerometer.h"
#include "Gyroscope.h"
#include "Servos.h"
#include "Load.h"

std::ostream & operator<<(std::ostream &stream, const msg_frame_calibration_t &calibration) {
    stream << std::setprecision(3) << std::fixed << std::showpos;

    stream << "magnetometer\n";
    stream << std::setw(8) << calibration.magnetometer[0] << std::setw(8) << calibration.magnetometer[1] << std::setw(8) << calibration.magnetometer[2] << std::setw(12) << calibration.magnetometer[9]  << "\n";
    stream << std::setw(8) << calibration.magnetometer[3] << std::setw(8) << calibration.magnetometer[4] << std::setw(8) << calibration.magnetometer[5] << std::setw(12) << calibration.magnetometer[10] << "\n";
    stream << std::setw(8) << calibration.magnetometer[6] << std::setw(8) << calibration.magnetometer[7] << std::setw(8) << calibration.magnetometer[8] << std::setw(12) << calibration.magnetometer[11] << "\n";
    stream << "\n";

    stream << "accelerometer\n";
    stream << std::setw(8) << calibration.accelerometer[0] << std::setw(8) << calibration.accelerometer[1] << std::setw(8) << calibration.accelerometer[2] << std::setw(12) << calibration.accelerometer[9]  << "\n";
    stream << std::setw(8) << calibration.accelerometer[3] << std::setw(8) << calibration.accelerometer[4] << std::setw(8) << calibration.accelerometer[5] << std::setw(12) << calibration.accelerometer[10] << "\n";
    stream << std::setw(8) << calibration.accelerometer[6] << std::setw(8) << calibration.accelerometer[7] << std::setw(8) << calibration.accelerometer[8] << std::setw(12) << calibration.accelerometer[11] << "\n";
    stream << "\n";

    stream << "gyroscope\n";
    stream << std::setw(8) << calibration.gyroscope[0] << "\n";
    stream << std::setw(8) << calibration.gyroscope[1] << "\n";
    stream << std::setw(8) << calibration.gyroscope[2] << "\n";
    stream << "\n";

    stream << "servos\n";
    stream << std::noshowpos;
    stream << std::setw(8) << calibration.servos[0] << std::setw(8) << calibration.servos[1]  << std::setw(8) << calibration.servos[2]  << "\n";
    stream << std::setw(8) << calibration.servos[3] << std::setw(8) << calibration.servos[4]  << std::setw(8) << calibration.servos[5]  << "\n";
    stream << std::setw(8) << calibration.servos[6] << std::setw(8) << calibration.servos[7]  << std::setw(8) << calibration.servos[8]  << "\n";
    stream << "\n";

    stream << "load cell\n";
    stream << std::setw(12) << calibration.load[0] << "\n";
    stream << std::setw(12) << calibration.load[1] << "\n";
    stream << "\n";

    return stream;
}

Window::Window(QWidget *parent) : QWidget{parent}, current{nullptr} {
    interfaces.push_back(new Magnetometer());
    interfaces.push_back(new Accelerometer());
    interfaces.push_back(new Gyroscope());
    interfaces.push_back(new Servos(this));
    interfaces.push_back(new Load());

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

        grid->addWidget(serialInterface, 1, 0);
        grid->addWidget(networkInterface, 2, 0);
    }

    {
        QGroupBox *group = new QGroupBox("applications");
        QHBoxLayout *layout  = new QHBoxLayout(group);

        group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        for(Interface *interface : interfaces) {
            QPushButton *button = new QPushButton(interface->getName(), group);

            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            connect(button, &QPushButton::clicked, std::bind(&Window::setCurrent, this, interface));

            layout->addWidget(button);
        }

        grid->addWidget(group, 0, 1);
    }

    {
        QGroupBox *group = new QGroupBox("interface");
        interface_layout = new QHBoxLayout(group);

        group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        grid->addWidget(group, 1, 1, 3, 1);
    }

    {
        QGroupBox *group = new QGroupBox("parameters");
        QVBoxLayout *layout  = new QVBoxLayout(group);

        group->setFixedWidth(350);

        QFont font("System", 10);
        font.setStyleHint(QFont::TypeWriter);

        calibration_text = new QTextEdit(group);
        calibration_text->setReadOnly(true);
        calibration_text->setFont(font);
        QPushButton *button_read = new QPushButton("read from device", group);
        QPushButton *button_update = new QPushButton("update parameters", group);
        QPushButton *button_set = new QPushButton("write into device", group);

        connect(button_read, &QPushButton::clicked, [&]() {
            calibration_text->setText("fetching...");
            transmit(MSG_ID_CALIBRATION, QByteArray());
        });

        connect(button_update, &QPushButton::clicked, [&]() {
            if(current) {
                current->update(calibration);

                std::ostringstream stream;
                stream << calibration;
                calibration_text->setText(QString::fromStdString(stream.str()));
            } else {
                calibration_text->setText("app not selected");
            }
        });

        connect(button_set, &QPushButton::clicked, [&]() {
            calibration_text->setText("saving...");
            transmit(MSG_ID_CALIBRATION, QByteArray(reinterpret_cast<const char *>(&calibration), sizeof(calibration)));
        });

        layout->addWidget(calibration_text);
        layout->addWidget(button_read);
        layout->addWidget(button_update);
        layout->addWidget(button_set);

        grid->addWidget(group, 0, 2, 4, 1);
    }
}

void Window::setCurrent(Interface *interface) {
    if(current) {
        interface_layout->removeWidget(current);
        delete current;
        current = nullptr;
    }

    if(interface) {
        current = interface->create();
    }

    if(current) {
        interface_layout->addWidget(current);
    }
}

void Window::receive(const uint8_t id, const double time, const QByteArray &payload) {
    (void)time;

    if(current && id==MSG_ID_SENSOR && payload.size()==sizeof(msg_frame_sensor_t)) {
        const msg_frame_sensor_t *sensor = reinterpret_cast<const msg_frame_sensor_t *>(payload.data());

        current->receive(*sensor);

        update();

        return;
    }

    if(id==MSG_ID_CALIBRATION && payload.size()==sizeof(msg_frame_calibration_t)) {
        memcpy(&calibration, payload.data(), sizeof(calibration));

        std::ostringstream stream;
        stream << calibration;
        calibration_text->setText(QString::fromStdString(stream.str()));
    }
}
