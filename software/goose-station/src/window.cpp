#include "window.h"
#include "escape_codes.h"
#include <iostream>
#include <QGroupBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QDebug>
#include <QTimer>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <cmath>

Window::Window(QWidget *parent) : QWidget(parent) {

    QGridLayout *layout = new QGridLayout(this);

    {
        QGroupBox *group = new QGroupBox("source");

        source = new widgets::Form({"IP address", "COM port"}, false, group);
        source->set(0, "10.42.0.220");
        source->set(1, "/dev/ttyACM0");

        telnet.changeAddress("10.42.0.220");

        layout->addWidget(group, 0, 0);

        connect(source, &widgets::Form::change, [this](int index, QString value) {
            if(index==0) {
                telnet.changeAddress(value);
            }

            if(index==1) {
                usb.changePort(value);
            }
        });
    }

    {
        QGroupBox *group = new QGroupBox("power monitor");

        power = new widgets::Form({"shunt voltage", "bus voltage", "current", "power", "battery"}, true, group);

        layout->addWidget(group, 3, 0);
    }

    {
        QGroupBox *group = new QGroupBox("others");

        others = new widgets::Form({"state", "ground pressure", "pressure", "distance", "motor velocity"}, true, group);

        layout->addWidget(group, 1, 0);
    }

    {
        QGroupBox *group = new QGroupBox("steering");
        QGridLayout *grid = new QGridLayout(group);

        QPushButton *cmd_start = new QPushButton("Start", this);
        QPushButton *cmd_land = new QPushButton("Land", this);
        QPushButton *cmd_abort = new QPushButton("Abort", this);
        QPushButton *cmd_reset = new QPushButton("Reset", this);

        grid->addWidget(cmd_start, 0, 0);
        grid->addWidget(cmd_land, 1, 0);
        grid->addWidget(cmd_abort, 2, 0);
        grid->addWidget(cmd_reset, 4, 0);

        layout->addWidget(group, 2, 0);

        QTimer *timer = new QTimer();
        timer->setInterval(20);
	    timer->start();

        connect(cmd_start, &QPushButton::clicked, [this]() {
            transmit(Transfer::encode(comm::Command::START, Transfer::ID::CONTROL_COMMAND));
        });

        connect(cmd_land, &QPushButton::clicked, [this]() {
            transmit(Transfer::encode(comm::Command::LAND, Transfer::ID::CONTROL_COMMAND));
        });

        connect(cmd_abort, &QPushButton::clicked, [this]() {
            transmit(Transfer::encode(comm::Command::ABORT, Transfer::ID::CONTROL_COMMAND));
        });

        connect(cmd_reset, &QPushButton::clicked, [this]() {
            transmit(Transfer::encode(comm::Command::RESET, Transfer::ID::CONTROL_COMMAND));
        });

        connect(timer, &QTimer::timeout, [this]() {
            comm::Controller::State setpoint;

            setpoint.rpy[0] = -30.f*deg2rad*gamepad.get(Gamepad::Analog::LX);
            setpoint.rpy[1] = +30.f*deg2rad*gamepad.get(Gamepad::Analog::LY);
            setpoint.rpy[2] = -90.f*deg2rad*gamepad.get(Gamepad::Analog::RX);
            setpoint.w[0] = 0.f;
            setpoint.w[1] = 0.f;
            setpoint.w[2] = 0.f;
            setpoint.z = -gamepad.get(Gamepad::Analog::RY) + 1.f;
            setpoint.vz = 0.f;

            transmit(Transfer::encode(setpoint, Transfer::ID::CONTROL_SETPOINT));

            if(gamepad.get(Gamepad::Button::CROSS_UP)) {
                transmit(Transfer::encode(comm::Command::START, Transfer::ID::CONTROL_COMMAND));
            }

            if(gamepad.get(Gamepad::Button::CROSS_DOWN)) {
                transmit(Transfer::encode(comm::Command::LAND, Transfer::ID::CONTROL_COMMAND));
            }

            if(gamepad.get(Gamepad::Button::CIRCLE_X)) {
                transmit(Transfer::encode(comm::Command::ABORT, Transfer::ID::CONTROL_COMMAND));
            }
        });
    }

    {
        QGroupBox *group = new QGroupBox("manual");
        QGridLayout *grid = new QGridLayout(group);

        QCheckBox *manual_switch = new QCheckBox("Manual Mode");

        QSlider *manual[5];
        manual[0] = new QSlider(Qt::Orientation::Vertical);
        manual[1] = new QSlider(Qt::Orientation::Vertical);
        manual[2] = new QSlider(Qt::Orientation::Vertical);
        manual[3] = new QSlider(Qt::Orientation::Vertical);
        manual[4] = new QSlider(Qt::Orientation::Vertical);

        manual[0]->setRange(-45, 45);
        manual[1]->setRange(-45, 45);
        manual[2]->setRange(-45, 45);
        manual[3]->setRange(-45, 45);
        manual[4]->setRange(0, 100);

        QTimer *timer = new QTimer();
        timer->setInterval(50);

        connect(timer, &QTimer::timeout, [this, manual]() {
            comm::Manual data;
            data.angles[0] = deg2rad*manual[0]->value();
            data.angles[1] = deg2rad*manual[1]->value();
            data.angles[2] = deg2rad*manual[2]->value();
            data.angles[3] = deg2rad*manual[3]->value();
            data.throttle = manual[4]->value()/100.f;

            transmit(Transfer::encode(data, Transfer::ID::CONTROL_MANUAL));
        });

        connect(manual_switch, &QPushButton::clicked, [timer, manual]() {
            manual[0]->setValue(0);
            manual[1]->setValue(0);
            manual[2]->setValue(0);
            manual[3]->setValue(0);
            manual[4]->setValue(0);

            if(timer->isActive()) {
                timer->stop();
            } else {
                timer->start();
            }
        });

        grid->addWidget(manual[0], 0, 0);
        grid->addWidget(manual[1], 0, 1);
        grid->addWidget(manual[2], 0, 2);
        grid->addWidget(manual[3], 0, 3);
        grid->addWidget(manual[4], 0, 4);
        grid->addWidget(manual_switch, 1, 0, 4, 0);
        layout->addWidget(group, 1, 1, 4, 1);
    }

    {
        altitude = new widgets::LiveChart(0, 2, this);
        altitude->addSeries("setpoint", QPen(Qt::black,   1, Qt::DashLine));
        altitude->addSeries("process", QPen(Qt::black,    2, Qt::SolidLine));

        layout->addWidget(altitude, 0, 4, 2, 1);
    }

    {
        attitude = new widgets::LiveChart(-180, 180, this);
        attitude->addSeries("X setpoint", QPen(Qt::red,     1, Qt::DashLine));
        attitude->addSeries("Y setpoint", QPen(Qt::green,   1, Qt::DashLine));
        attitude->addSeries("Z setpoint", QPen(Qt::blue,    1, Qt::DashLine));
        attitude->addSeries("X process", QPen(Qt::red,      2, Qt::SolidLine));
        attitude->addSeries("Y process", QPen(Qt::green,    2, Qt::SolidLine));
        attitude->addSeries("Z process", QPen(Qt::blue,     2, Qt::SolidLine));

        layout->addWidget(attitude, 2, 4, 2, 1);
    }

    {
        throttle = new widgets::LiveChart(0, 100, this);
        throttle->addSeries("throttle", QPen(Qt::black, 2, Qt::SolidLine));

        layout->addWidget(throttle, 0, 3, 2, 1);
    }

    {
        fins = new widgets::LiveChart(-20, 20, this);
        fins->addSeries("fin 1", QPen(Qt::red,      2, Qt::SolidLine));
        fins->addSeries("fin 2", QPen(Qt::green,    2, Qt::SolidLine));
        fins->addSeries("fin 3", QPen(Qt::blue,     2, Qt::SolidLine));
        fins->addSeries("fin 4", QPen(Qt::magenta,  2, Qt::SolidLine));

        layout->addWidget(fins, 2, 3, 2, 1);
    }

    {
        QPushButton *resume = new QPushButton("Resume", this);

        layout->addWidget(resume, 0, 1);

        connect(resume, &QPushButton::clicked, [this]() {
            altitude->resume();
            attitude->resume();
            throttle->resume();
            fins->resume();
        });
    }

    connect(&usb, &USB::receive, this, &Window::receiveCallback);
    connect(&telnet, &Telnet::receive, this, &Window::receiveCallback);
    connect(this, &Window::transmit, &usb, &USB::transmit);
    connect(this, &Window::transmit, &telnet, &Telnet::transmit);
}

void Window::receiveCallback(Transfer::FrameRX frame) {

    if(frame.id==Transfer::ID::TELEMETRY_ESTIMATOR) {
        comm::Estimator estimator_data;
        frame.getPayload(estimator_data);

        power->set(4, "%1.2f", estimator_data.soc);

        others->set(1, "%6.0f", estimator_data.ground_pressure);
    }

    if(frame.id==Transfer::ID::TELEMETRY_CONTROLLER) {
        comm::Controller controller_data;
        frame.getPayload(controller_data);

        switch(controller_data.state) {
            case comm::Controller::SMState::ABORT:      others->set(0, "abort");     break;
            case comm::Controller::SMState::READY:      others->set(0, "ready");     break;
            case comm::Controller::SMState::ACTIVE:     others->set(0, "active");    break;
            case comm::Controller::SMState::LANDING:    others->set(0, "landing");   break;
        }

        if(controller_data.state==comm::Controller::SMState::ABORT) {
            altitude->pause();
            attitude->pause();
            throttle->pause();
            fins->pause();
        }

        throttle->append("throttle", 100*controller_data.throttle);
        fins->append("fin 1", rad2deg*controller_data.angles[0]);
        fins->append("fin 2", rad2deg*controller_data.angles[1]);
        fins->append("fin 3", rad2deg*controller_data.angles[2]);
        fins->append("fin 4", rad2deg*controller_data.angles[3]);

        altitude->append("setpoint", controller_data.setpoint.z);
        altitude->append("process", controller_data.process_value.z);

        attitude->append("X setpoint", rad2deg*controller_data.setpoint.rpy[0]);
        attitude->append("Y setpoint", rad2deg*controller_data.setpoint.rpy[1]);
        attitude->append("Z setpoint", rad2deg*controller_data.setpoint.rpy[2]);
        attitude->append("X process", rad2deg*controller_data.process_value.rpy[0]);
        attitude->append("Y process", rad2deg*controller_data.process_value.rpy[1]);
        attitude->append("Z process", rad2deg*controller_data.process_value.rpy[2]);
    }

    if(frame.id<=Transfer::ID::LOG_ERROR) {
		switch(frame.id) {
			case Transfer::ID::LOG_DEBUG:	std::cout << EscapeCode::GRAY;		break;
			case Transfer::ID::LOG_INFO:	std::cout << EscapeCode::CYAN;		break;
			case Transfer::ID::LOG_WARNING:	std::cout << EscapeCode::YELLOW;	break;
			case Transfer::ID::LOG_ERROR:	std::cout << EscapeCode::RED;		break;
			default: break;
		}

		std::cout << std::string(reinterpret_cast<const char *>(frame.payload), frame.length) << std::endl;

		return;
	}

    if(frame.id==Transfer::ID::SENSOR_POWER_MONITOR) {
        comm::Power monitor;
        frame.getPayload(monitor);

        power->set(0, "%2.6f", monitor.shunt);
        power->set(1, "%2.2f", monitor.bus);
        power->set(2, "%2.2f", monitor.current);
        power->set(3, "%2.2f", monitor.power);
    }

    if(frame.id==Transfer::ID::SENSOR_PRESSURE) {
        float pressure;
        frame.getPayload(pressure);
        others->set(2, "%6.0f", pressure);
    }

    if(frame.id==Transfer::ID::SENSOR_DISTANCE) {
        float distance;
        frame.getPayload(distance);
        others->set(3, "%6.3f", distance);
    }

    if(frame.id==Transfer::ID::SENSOR_MOTOR_VELOCITY) {
        float velocity;
        frame.getPayload(velocity);
        others->set(4, "%6.3f", velocity);
    }
}
