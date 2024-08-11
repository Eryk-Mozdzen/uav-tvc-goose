#include <sstream>
#include <iomanip>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>

#include "common/protocol/protocol_data.h"
#include "Window.h"
#include "Magnetometer.h"
#include "Accelerometer.h"
#include "Gyroscope.h"
#include "Servos.h"

std::ostream & operator<<(std::ostream &stream, const protocol_calibration_t &calibration) {
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
    stream << std::setw(8) << calibration.servos[9] << std::setw(8) << calibration.servos[10] << std::setw(8) << calibration.servos[11] << "\n";

    return stream;
}

Window::Window(QWidget *parent) : QWidget{parent}, current{nullptr} {
    interfaces.push_back(new Magnetometer());
    interfaces.push_back(new Accelerometer());
    interfaces.push_back(new Gyroscope());
    interfaces.push_back(new Servos(this));

    QGridLayout *grid = new QGridLayout(this);

    {
        common::Serial *serial = new common::Serial(this);
	    common::Network *network = new common::Network(this);

        connect(serial, &common::Serial::receive, this, &Window::receive);
        connect(network, &common::Network::receive, this, &Window::receive);
        connect(this, &Window::transmit, serial, &common::Serial::transmit);
        connect(this, &Window::transmit, network, &common::Network::transmit);

        grid->addWidget(serial, 0, 0);
        grid->addWidget(network, 1, 0);
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

        grid->addWidget(group, 1, 1, 2, 1);
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
            transmit(PROTOCOL_ID_CALIBRATION, NULL, 0);
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
            transmit(PROTOCOL_ID_CALIBRATION, &calibration, sizeof(calibration));
        });

        layout->addWidget(calibration_text);
        layout->addWidget(button_read);
        layout->addWidget(button_update);
        layout->addWidget(button_set);

        grid->addWidget(group, 0, 2, 3, 1);
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

void Window::receive(const uint8_t id, const void *payload, const uint32_t size) {
    if(current && id==PROTOCOL_ID_READINGS && size==sizeof(protocol_readings_t)) {
        const protocol_readings_t *readings = reinterpret_cast<const protocol_readings_t *>(payload);

        current->receive(*readings);

        update();

        return;
    }

    if(id==PROTOCOL_ID_CALIBRATION && size==sizeof(protocol_calibration_t)) {
        memcpy(&calibration, payload, sizeof(calibration));

        std::ostringstream stream;
        stream << calibration;
        calibration_text->setText(QString::fromStdString(stream.str()));
    }
}
