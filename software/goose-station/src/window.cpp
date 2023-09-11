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

        layout->addWidget(group, 1, 0);
    }

    {
        QGroupBox *group = new QGroupBox("others");

        others = new widgets::Form({"state", "ground pressure", "pressure", "distance", "motor velocity"}, true, group);

        layout->addWidget(group, 2, 0);
    }

    {
        QGroupBox *group = new QGroupBox("setpoint");

        setpoint = new widgets::Form({"roll", "pitch", "yaw", "Wx", "Wy", "Wz", "z", "Vz"}, true, group);

        layout->addWidget(group, 0, 1, 2, 1);
    }

    {
        QGroupBox *group = new QGroupBox("process");

        process = new widgets::Form({"roll", "pitch", "yaw", "Wx", "Wy", "Wz", "z", "Vz"}, true, group);

        layout->addWidget(group, 0, 2, 2, 1);
    }

    {
        QGroupBox *group = new QGroupBox("steering");
        QGridLayout *grid = new QGridLayout(group);

        QComboBox *freq = new QComboBox();
        freq->addItem("off");
        freq->addItem("0.5 Hz");
        freq->addItem("1 Hz");
        freq->addItem("2 Hz");
        freq->addItem("50 Hz");
        freq->setCurrentIndex(4);

        QPushButton *cmd_start = new QPushButton("Start Command", this);
        QPushButton *cmd_land = new QPushButton("Land Command", this);
        QPushButton *cmd_abort = new QPushButton("Abort Command", this);

        grid->addWidget(freq, 0, 0, 1, 2);
        grid->addWidget(cmd_start, 1, 0);
        grid->addWidget(cmd_land, 1, 1);
        grid->addWidget(cmd_abort, 2, 0, 1, 2);

        layout->addWidget(group, 2, 1);

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

        connect(freq, &QComboBox::textActivated, [timer](QString value) {
            if(value=="off") {
                timer->stop();
            }

            if(value=="0.5 Hz") {
                timer->setInterval(2000);
                timer->start();
            }

            if(value=="1 Hz") {
                timer->setInterval(1000);
                timer->start();
            }

            if(value=="2 Hz") {
                timer->setInterval(500);
                timer->start();
            }

            if(value=="50 Hz") {
                timer->setInterval(20);
                timer->start();
            }
        });
    }

    {
        QGroupBox *group = new QGroupBox("actuators");

        actuators = new widgets::Form({"fin 1", "fin 2", "fin 3", "fin 4", "throttle"}, true, group);

        layout->addWidget(group, 2, 2);
    }

    {
        QGroupBox *group = new QGroupBox("position");

        position = new widgets::Form({"x", "y", "z"}, true, group);

        layout->addWidget(group, 3, 0);
    }

    {
        QGroupBox *group = new QGroupBox("velocity");

        velocity = new widgets::Form({"x", "y", "z"}, true, group);

        layout->addWidget(group, 3, 1);
    }

    {
        QGroupBox *group = new QGroupBox("acceleration");

        acceleration = new widgets::Form({"x", "y", "z"}, true, group);

        layout->addWidget(group, 3, 2);
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
        layout->addWidget(group, 0, 4, 4, 4);
    }

    {
        altitude = new widgets::LiveChart(0, 2, this);
        altitude->addSeries("setpoint", QPen(Qt::black,   1, Qt::DashLine));
        altitude->addSeries("process", QPen(Qt::black,    2, Qt::SolidLine));

        layout->addWidget(altitude, 0, 8, 2, 1);
    }

    {
        attitude = new widgets::LiveChart(-90, 90, this);
        attitude->addSeries("X setpoint", QPen(Qt::red,     1, Qt::DashLine));
        attitude->addSeries("Y setpoint", QPen(Qt::green,   1, Qt::DashLine));
        attitude->addSeries("Z setpoint", QPen(Qt::blue,    1, Qt::DashLine));
        attitude->addSeries("X process", QPen(Qt::red,      2, Qt::SolidLine));
        attitude->addSeries("Y process", QPen(Qt::green,    2, Qt::SolidLine));
        attitude->addSeries("Z process", QPen(Qt::blue,     2, Qt::SolidLine));

        layout->addWidget(attitude, 2, 8, 2, 1);
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

        position->set(0, "%+3.2f", estimator_data.position[0]);
        position->set(1, "%+3.2f", estimator_data.position[1]);
        position->set(2, "%+3.2f", estimator_data.position[2]);

        velocity->set(0, "%+3.2f", estimator_data.velocity[0]);
        velocity->set(1, "%+3.2f", estimator_data.velocity[1]);
        velocity->set(2, "%+3.2f", estimator_data.velocity[2]);

        acceleration->set(0, "%+3.2f", estimator_data.acceleration[0]);
        acceleration->set(1, "%+3.2f", estimator_data.acceleration[1]);
        acceleration->set(2, "%+3.2f", estimator_data.acceleration[2]);

        others->set(1, "%6.0f", estimator_data.ground_pressure);
    }

    if(frame.id==Transfer::ID::TELEMETRY_CONTROLLER) {
        comm::Controller controller_data;
        frame.getPayload(controller_data);

        setpoint->set(0, "%+3.0f", rad2deg*controller_data.setpoint.rpy[0]);
        setpoint->set(1, "%+3.0f", rad2deg*controller_data.setpoint.rpy[1]);
        setpoint->set(2, "%+3.0f", rad2deg*controller_data.setpoint.rpy[2]);
        setpoint->set(3, "%+3.0f", rad2deg*controller_data.setpoint.w[0]);
        setpoint->set(4, "%+3.0f", rad2deg*controller_data.setpoint.w[1]);
        setpoint->set(5, "%+3.0f", rad2deg*controller_data.setpoint.w[2]);
        setpoint->set(6, "%+2.2f", controller_data.setpoint.z);
        setpoint->set(7, "%+2.2f", controller_data.setpoint.vz);

        process->set(0, "%+3.0f", rad2deg*controller_data.process_value.rpy[0]);
        process->set(1, "%+3.0f", rad2deg*controller_data.process_value.rpy[1]);
        process->set(2, "%+3.0f", rad2deg*controller_data.process_value.rpy[2]);
        process->set(3, "%+3.0f", rad2deg*controller_data.process_value.w[0]);
        process->set(4, "%+3.0f", rad2deg*controller_data.process_value.w[1]);
        process->set(5, "%+3.0f", rad2deg*controller_data.process_value.w[2]);
        process->set(6, "%+2.2f", controller_data.process_value.z);
        process->set(7, "%+2.2f", controller_data.process_value.vz);

        actuators->set(0, "%+3.0f", rad2deg*controller_data.angles[0]);
        actuators->set(1, "%+3.0f", rad2deg*controller_data.angles[1]);
        actuators->set(2, "%+3.0f", rad2deg*controller_data.angles[2]);
        actuators->set(3, "%+3.0f", rad2deg*controller_data.angles[3]);
        actuators->set(4, "%1.2f", controller_data.throttle);

        switch(controller_data.state) {
            case comm::Controller::SMState::ABORT:      others->set(0, "abort");     break;
            case comm::Controller::SMState::READY:      others->set(0, "ready");     break;
            case comm::Controller::SMState::ACTIVE:     others->set(0, "active");    break;
            case comm::Controller::SMState::LANDING:    others->set(0, "landing");   break;
        }

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
