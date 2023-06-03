#include "window.h"
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
        setpoint{"setpoint", {"roll", "pitch", "yaw", "z"}},
        process{"current", {"roll", "pitch", "yaw", "Wx", "Wy", "Wz", "z", "Vz"}},
        actuators{"actuators", {"fin 1", "fin 2", "fin 3", "fin 4", "throttle"}},
        others{"others", {"state", "altitude src"}} {

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

        layout->addWidget(group, 1, 0);
    }

    layout->addWidget(&source, 0, 0);
    layout->addWidget(&setpoint, 0, 1);
    layout->addWidget(&process, 0, 2);
    layout->addWidget(&others, 1, 1);
    layout->addWidget(&actuators, 1, 2);

    source.set(0, "192.168.0.13");
    source.set(1, "/dev/ttyACM0");

	timer.setInterval(20);
	timer.start();

    connect(&timer, &QTimer::timeout, this, &Window::sendSetpoint);
    connect(freq, &QComboBox::textActivated, this, &Window::freqChanged);
    connect(cmd_start, &QPushButton::clicked, this, std::bind(&Window::sendCommand, this, Transfer::Command::START));
    connect(cmd_land, &QPushButton::clicked, this, std::bind(&Window::sendCommand, this, Transfer::Command::LAND));
    connect(&source, &widgets::Form::change, this, &Window::sourceChanged);
    connect(&usb, &USB::receive, this, &Window::frameReceived);
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
    Transfer::Setpoint setpoint;

    setpoint.rpy[0] = -30.f*deg2rad*gamepad.get(Gamepad::Analog::LX);
    setpoint.rpy[1] = +30.f*deg2rad*gamepad.get(Gamepad::Analog::LY);
    setpoint.rpy[2] = +90.f*deg2rad*gamepad.get(Gamepad::Analog::RX);
    setpoint.z = 1.f - gamepad.get(Gamepad::Analog::RY);

    this->setpoint.set(0, "%+3.0f", rad2deg*setpoint.rpy[0]);
    this->setpoint.set(1, "%+3.0f", rad2deg*setpoint.rpy[1]);
    this->setpoint.set(2, "%+3.0f", rad2deg*setpoint.rpy[2]);
    this->setpoint.set(3, "%+2.2f", setpoint.z);

    transmit(Transfer::encode(setpoint, Transfer::ID::CONTROL_SETPOINT));
}

void Window::sendCommand(Transfer::Command cmd) {
    transmit(Transfer::encode(cmd, Transfer::ID::CONTROL_COMMAND));
}

void Window::frameReceived(Transfer::FrameRX frame) {

    if(frame.id==Transfer::ID::STATE) {
        Transfer::State state;
        frame.getPayload(state);

        process.set(0, "%+3.0f", rad2deg*state.rpy[0]);
        process.set(1, "%+3.0f", rad2deg*state.rpy[1]);
        process.set(2, "%+3.0f", rad2deg*state.rpy[2]);
        process.set(3, "%+3.0f", rad2deg*state.omega[0]);
        process.set(4, "%+3.0f", rad2deg*state.omega[1]);
        process.set(5, "%+3.0f", rad2deg*state.omega[2]);
        process.set(6, "%+2.2f", state.z);
        process.set(7, "%+2.2f", state.vz);

        switch(state.alt_src) {
            case Transfer::AltitudeSource::DISTANCE: others.set(1, "distance"); break;
            case Transfer::AltitudeSource::PRESSURE: others.set(1, "pressure"); break;
        }
    }

    if(frame.id==Transfer::ID::CONTROLLER) {
        Transfer::Controller controller;
        frame.getPayload(controller);

        actuators.set(0, "%+3.0f", rad2deg*controller.angles[0]);
        actuators.set(1, "%+3.0f", rad2deg*controller.angles[1]);
        actuators.set(2, "%+3.0f", rad2deg*controller.angles[2]);
        actuators.set(3, "%+3.0f", rad2deg*controller.angles[3]);
        actuators.set(4, "%1.2f", controller.throttle);

        switch(controller.sm_state) {
            case Transfer::SMState::ABORT:      others.set(0, "abort");     break;
            case Transfer::SMState::READY:      others.set(0, "ready");     break;
            case Transfer::SMState::TAKE_OFF:   others.set(0, "take off");  break;
            case Transfer::SMState::ACTIVE:     others.set(0, "active");    break;
            case Transfer::SMState::LANDING:    others.set(0, "landing");   break;
        }
    }
}
