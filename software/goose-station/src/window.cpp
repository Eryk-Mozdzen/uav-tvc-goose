#include "window.h"
#include "escape_codes.h"
#include <iostream>
#include <QGroupBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <cmath>

Window::Window(QWidget *parent) : QWidget(parent),
        source{"source", {"IP address", "COM port"}, false},
        setpoint{"setpoint", {"roll", "pitch", "yaw", "Wx", "Wy", "Wz", "z", "Vz"}},
        process{"process value", {"roll", "pitch", "yaw", "Wx", "Wy", "Wz", "z", "Vz"}},
        actuators{"actuators", {"fin 1", "fin 2", "fin 3", "fin 4", "throttle"}},
        others{"others", {"state", "altitude src"}},
        power{"power", {"voltage", "current", "battery"}} {

    QGridLayout *layout = new QGridLayout(this);

    {
        freq = new QComboBox();
        cmd_start = new QPushButton("Start Command", this);
        cmd_land = new QPushButton("Land Command", this);

        QGroupBox *group = new QGroupBox("steering");
        QGridLayout *grid = new QGridLayout(group);

        freq->addItem("off");
        freq->addItem("0.5 Hz");
        freq->addItem("1 Hz");
        freq->addItem("2 Hz");
        freq->addItem("50 Hz");
        freq->setCurrentIndex(4);

        grid->addWidget(freq, 0, 0, 1, 2);
        grid->addWidget(cmd_start, 1, 0);
        grid->addWidget(cmd_land, 1, 1);

        layout->addWidget(group, 2, 1);
    }

    layout->addWidget(&source, 0, 0);
    layout->addWidget(&power, 1, 0);
    layout->addWidget(&others, 2, 0);

    layout->addWidget(&setpoint, 0, 1, 2, 1);
    layout->addWidget(&process, 0, 2, 2, 1);
    layout->addWidget(&actuators, 2, 2);

    source.set(0, "192.168.101.29");
    source.set(1, "/dev/ttyACM0");

	timer.setInterval(20);
	timer.start();

    connect(&timer, &QTimer::timeout, this, &Window::sendSetpoint);
    connect(freq, &QComboBox::textActivated, this, &Window::freqChanged);
    connect(cmd_start, &QPushButton::clicked, this, std::bind(&Window::sendCommand, this, comm::Command::START));
    connect(cmd_land, &QPushButton::clicked, this, std::bind(&Window::sendCommand, this, comm::Command::LAND));
    connect(&source, &widgets::Form::change, this, &Window::sourceChanged);
    connect(&usb, &USB::receive, this, &Window::frameReceived);
    connect(&telnet, &Telnet::receive, this, &Window::frameReceived);
    connect(this, &Window::transmit, &usb, &USB::transmit);
    connect(this, &Window::transmit, &telnet, &Telnet::transmit);
}

void Window::freqChanged(QString value) {
    if(value=="off") {
        timer.stop();
    }

    if(value=="0.5 Hz") {
        timer.setInterval(2000);
	    timer.start();
    }

    if(value=="1 Hz") {
        timer.setInterval(1000);
	    timer.start();
    }

    if(value=="2 Hz") {
        timer.setInterval(500);
	    timer.start();
    }

    if(value=="50 Hz") {
        timer.setInterval(20);
	    timer.start();
    }
}

void Window::sourceChanged(int index, QString value) {
    if(index==0) {
        telnet.changeAddress(value);
    }

    if(index==1) {
        usb.changePort(value);
    }
}

void Window::sendSetpoint() {
    comm::Controller::State setpoint;

    setpoint.rpy[0] = -30.f*deg2rad*gamepad.get(Gamepad::Analog::LX);
    setpoint.rpy[1] = +30.f*deg2rad*gamepad.get(Gamepad::Analog::LY);
    setpoint.rpy[2] = 0.f;
    setpoint.w[0] = 0.f;
    setpoint.w[1] = 0.f;
    setpoint.w[2] = 90.f*deg2rad*gamepad.get(Gamepad::Analog::RX);
    setpoint.z = 1.f - gamepad.get(Gamepad::Analog::RY);
    setpoint.vz = 0.f;

    transmit(Transfer::encode(setpoint, Transfer::ID::CONTROL_SETPOINT));
}

void Window::sendCommand(comm::Command cmd) {
    transmit(Transfer::encode(cmd, Transfer::ID::CONTROL_COMMAND));
}

void Window::frameReceived(Transfer::FrameRX frame) {

    if(frame.id==Transfer::ID::TELEMETRY_ESTIMATOR) {
        comm::Estimator estimator_data;
        frame.getPayload(estimator_data);

        power.set(2, "%1.2f", estimator_data.battery_soc);

        switch(estimator_data.altitude_src) {
            case comm::Estimator::AltitudeSource::DISTANCE: others.set(1, "distance"); break;
            case comm::Estimator::AltitudeSource::PRESSURE: others.set(1, "pressure"); break;
        }
    }

    if(frame.id==Transfer::ID::TELEMETRY_CONTROLLER) {
        comm::Controller controller_data;
        frame.getPayload(controller_data);

        setpoint.set(0, "%+3.0f", rad2deg*controller_data.setpoint.rpy[0]);
        setpoint.set(1, "%+3.0f", rad2deg*controller_data.setpoint.rpy[1]);
        setpoint.set(2, "%+3.0f", rad2deg*controller_data.setpoint.rpy[2]);
        setpoint.set(3, "%+3.0f", rad2deg*controller_data.setpoint.w[0]);
        setpoint.set(4, "%+3.0f", rad2deg*controller_data.setpoint.w[1]);
        setpoint.set(5, "%+3.0f", rad2deg*controller_data.setpoint.w[2]);
        setpoint.set(6, "%+2.2f", controller_data.setpoint.z);
        setpoint.set(7, "%+2.2f", controller_data.setpoint.vz);

        process.set(0, "%+3.0f", rad2deg*controller_data.process_value.rpy[0]);
        process.set(1, "%+3.0f", rad2deg*controller_data.process_value.rpy[1]);
        process.set(2, "%+3.0f", rad2deg*controller_data.process_value.rpy[2]);
        process.set(3, "%+3.0f", rad2deg*controller_data.process_value.w[0]);
        process.set(4, "%+3.0f", rad2deg*controller_data.process_value.w[1]);
        process.set(5, "%+3.0f", rad2deg*controller_data.process_value.w[2]);
        process.set(6, "%+2.2f", controller_data.process_value.z);
        process.set(7, "%+2.2f", controller_data.process_value.vz);

        actuators.set(0, "%+3.0f", rad2deg*controller_data.angles[0]);
        actuators.set(1, "%+3.0f", rad2deg*controller_data.angles[1]);
        actuators.set(2, "%+3.0f", rad2deg*controller_data.angles[2]);
        actuators.set(3, "%+3.0f", rad2deg*controller_data.angles[3]);
        actuators.set(4, "%1.2f", controller_data.throttle);

        switch(controller_data.state) {
            case comm::Controller::SMState::ABORT:      others.set(0, "abort");     break;
            case comm::Controller::SMState::READY:      others.set(0, "ready");     break;
            case comm::Controller::SMState::TAKE_OFF:   others.set(0, "take off");  break;
            case comm::Controller::SMState::ACTIVE:     others.set(0, "active");    break;
            case comm::Controller::SMState::LANDING:    others.set(0, "landing");   break;
        }
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

    if(frame.id==Transfer::ID::SENSOR_VOLTAGE) {
        float voltage;
        frame.getPayload(voltage);
        power.set(0, "%2.2f", voltage);
    }

    if(frame.id==Transfer::ID::SENSOR_CURRENT) {
        float current;
        frame.getPayload(current);
        power.set(1, "%2.2f", current);
    }
}
