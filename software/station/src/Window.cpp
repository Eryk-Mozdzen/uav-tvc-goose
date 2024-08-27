#include <iostream>
#include <iomanip>
#include <cmath>

#include <QGroupBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QTimer>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QSettings>
#include <QThread>

#include "common/math/utils.h"
#include "common/protocol/msg.h"
#include "common/qt/Serial.h"
#include "common/qt/Network.h"
#include "common/qt/InterfaceWidget.h"
#include "Window.h"
#include "Form.h"
#include "LiveChart.h"
#include "Visualizer.h"

std::ostream & operator<<(std::ostream &stream, const msg_frame_sensor_t sensor) {
	stream << "press";
	stream << std::setprecision(0) << std::fixed << std::noshowpos << std::setfill(' ') << std::setw(7);
	stream << (sensor.valid.barometer ? sensor.barometer : std::nan(""));

	stream << "   range";
	stream << std::setprecision(2) << std::fixed << std::noshowpos << std::setfill(' ') << std::setw(5);
	stream << (sensor.valid.rangefinder ? sensor.rangefinder : std::nan(""));

	stream << "   mag [";
	stream << std::setprecision(2) << std::fixed << std::showpos << std::setfill(' ');
	stream << std::setw(6) << (sensor.valid.magnetometer ? sensor.magnetometer.calib[0] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.magnetometer ? sensor.magnetometer.calib[1] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.magnetometer ? sensor.magnetometer.calib[2] : std::nan(""));
	stream << "]";

	stream << "   accel [";
	stream << std::setprecision(2) << std::fixed << std::showpos << std::setfill(' ');
	stream << std::setw(6) << (sensor.valid.accelerometer ? sensor.accelerometer.calib[0] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.accelerometer ? sensor.accelerometer.calib[1] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.accelerometer ? sensor.accelerometer.calib[2] : std::nan(""));
	stream << "]";

	stream << "   gyro [";
	stream << std::setprecision(2) << std::fixed << std::showpos << std::setfill(' ');
	stream << std::setw(6) << (sensor.valid.gyroscope ? sensor.gyroscope.calib[0] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.gyroscope ? sensor.gyroscope.calib[1] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.gyroscope ? sensor.gyroscope.calib[2] : std::nan(""));
	stream << "]";

	stream << "   gps [";
	stream << std::setprecision(6) << std::fixed << std::noshowpos << std::setfill(' ');
	stream << std::setw(10) << (sensor.valid.gps ? sensor.gps[0] : std::nan(""));
	stream << std::setw(10) << (sensor.valid.gps ? sensor.gps[1] : std::nan(""));
	stream << "]";

    stream << "   power [";
	stream << std::setprecision(2) << std::fixed << std::noshowpos << std::setfill(' ');
	stream << std::setw(6) << (sensor.valid.power ? sensor.power[0] : std::nan("")) << "V";
	stream << std::setw(6) << (sensor.valid.power ? sensor.power[1] : std::nan("")) << "A";
	stream << "]";

	return stream;
}

std::ostream & operator<<(std::ostream &stream, const msg_frame_estimation_t estimation) {
	stream << "qua [";
	stream << std::setprecision(2) << std::fixed << std::showpos << std::setfill(' ');
	stream << std::setw(6) << estimation.orientation[0];
	stream << std::setw(6) << estimation.orientation[1];
	stream << std::setw(6) << estimation.orientation[2];
	stream << std::setw(6) << estimation.orientation[3];
	stream << "]";

	stream << "   pos [";
	stream << std::setprecision(2) << std::fixed << std::showpos << std::setfill(' ');
	stream << std::setw(6) << estimation.position[0];
	stream << std::setw(6) << estimation.position[1];
	stream << std::setw(6) << estimation.position[2];
	stream << "]";

	stream << "   vel [";
	stream << std::setprecision(2) << std::fixed << std::showpos << std::setfill(' ');
	stream << std::setw(6) << estimation.velocity[0];
	stream << std::setw(6) << estimation.velocity[1];
	stream << std::setw(6) << estimation.velocity[2];
	stream << "]";

	return stream;
}

Window::Window(QWidget *parent) : QWidget(parent) {
    QGridLayout *layout = new QGridLayout(this);

    common::Serial *serial = new common::Serial();
    common::Network *network = new common::Network();
    Visualizer *visualizer = new Visualizer();

    connect(serial, &common::Serial::receive, this, &Window::receive);
    connect(network, &common::Network::receive, this, &Window::receive);
    connect(serial, &common::Serial::receive, visualizer, &Visualizer::receive);
    connect(network, &common::Network::receive, visualizer, &Visualizer::receive);
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
    QThread *visualizerThread = new QThread(this);

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

    visualizer->moveToThread(visualizerThread);
    connect(visualizerThread, &QThread::started, visualizer, &Visualizer::start);
    connect(visualizerThread, &QThread::finished, visualizer, &Visualizer::deleteLater);
    connect(visualizerThread, &QThread::finished, visualizerThread, &QThread::deleteLater);
    connect(this, &QObject::destroyed, visualizerThread, &QThread::quit);

    serialThread->start();
    networkThread->start();
    visualizerThread->start();

    serialInterface->forceScan();
    networkInterface->forceScan();

    layout->addWidget(networkInterface, 0, 0, 1, 2);
    layout->addWidget(serialInterface, 0, 2);
    layout->addWidget(&gamepad, 0, 3);

    {
        others = new Form("Others", {
            "State machine",
            "Magnetic inclination",
            "Ground pressure",
            "Pressure",
            "Distance",
            "Rotor velocity",
            "Supply voltage",
            "Supply current"
        });

        layout->addWidget(others, 1, 0);
    }

    {
        QGroupBox *group = new QGroupBox("Controls", this);
        QVBoxLayout *inner = new QVBoxLayout(group);

        QPushButton *cmd_start = new QPushButton("Start command", this);
        QPushButton *cmd_abort = new QPushButton("Abort command", this);
        QPushButton *resume = new QPushButton("Resume plots", this);
        QPushButton *save = new QPushButton("Save plots", this);
        QPushButton *spawnDark = new QPushButton("Spawn server (dark)", this);
        QPushButton *spawnLight = new QPushButton("Spawn server (light)", this);
        QRadioButton *source1 = new QRadioButton("Logger", this);
        QRadioButton *source2 = new QRadioButton("GPS passthrough", this);
        QRadioButton *source3 = new QRadioButton("Sensor readings", this);
        QRadioButton *source4 = new QRadioButton("Estimation", this);

        inner->addWidget(cmd_start);
        inner->addWidget(cmd_abort);
        inner->addWidget(resume);
        inner->addWidget(save);
        inner->addWidget(spawnDark);
        inner->addWidget(spawnLight);
        inner->addWidget(source1);
        inner->addWidget(source2);
        inner->addWidget(source3);
        inner->addWidget(source4);

        layout->addWidget(group, 2, 0);

        connect(cmd_start, &QPushButton::clicked, [this]() {
            transmit(MSG_ID_COMMAND_START, QByteArray());
        });

        connect(cmd_abort, &QPushButton::clicked, [this]() {
            transmit(MSG_ID_COMMAND_ABORT, QByteArray());
        });

        connect(resume, &QPushButton::clicked, []() {
            LiveChart::resume();
        });

        connect(save, &QPushButton::clicked, []() {
            LiveChart::save();
        });

        connect(spawnDark, &QPushButton::clicked, visualizer, &Visualizer::spawnDark);
        connect(spawnDark, &QPushButton::clicked, [spawnLight, spawnDark]() {
            spawnLight->setDisabled(true);
            spawnDark->setDisabled(true);
        });

        connect(spawnLight, &QPushButton::clicked, visualizer, &Visualizer::spawnLight);
        connect(spawnLight, &QPushButton::clicked, [spawnLight, spawnDark]() {
            spawnLight->setDisabled(true);
            spawnDark->setDisabled(true);
        });

        connect(visualizer, &Visualizer::finished, [spawnLight, spawnDark]() {
            spawnLight->setDisabled(false);
            spawnDark->setDisabled(false);
        });

        switch(settings.value("terminalSource").toInt()) {
            case 1: {
                source1->setChecked(true);
            } break;
            case 2: {
                source2->setChecked(true);
            } break;
            case 3: {
                source3->setChecked(true);
            } break;
            case 4: {
                source4->setChecked(true);
            } break;
        }

        connect(source1, &QRadioButton::clicked, [this]() {
            settings.setValue("terminalSource", 1);
        });

        connect(source2, &QRadioButton::clicked, [this]() {
            settings.setValue("terminalSource", 2);
        });

        connect(source3, &QRadioButton::clicked, [this]() {
            settings.setValue("terminalSource", 3);
        });

        connect(source4, &QRadioButton::clicked, [this]() {
            settings.setValue("terminalSource", 4);
        });
    }

    {
        QGroupBox *group = new QGroupBox("Manual");
        QGridLayout *grid = new QGridLayout(group);

        QCheckBox *manual_switch = new QCheckBox("Active");

        QSlider *manual[4];
        manual[0] = new QSlider(Qt::Orientation::Vertical);
        manual[1] = new QSlider(Qt::Orientation::Vertical);
        manual[2] = new QSlider(Qt::Orientation::Vertical);
        manual[3] = new QSlider(Qt::Orientation::Vertical);

        manual[0]->setRange(-100, 100);
        manual[1]->setRange(-100, 100);
        manual[2]->setRange(-100, 100);
        manual[3]->setRange(0, 100);

        QTimer *timer = new QTimer();
        connect(timer, &QTimer::timeout, [this, manual, manual_switch]() {
            if(manual_switch->checkState()==Qt::CheckState::Checked) {
                const float mx = manual[0]->value();
                const float my = manual[1]->value();
                const float mz = manual[2]->value();
                const float ur = manual[3]->value();

                constexpr float C = 0.01;

                msg_frame_manual_t frame;
                frame.is_raw = 0;
                frame.servos.calibrated[0] = C*(-0.333*mx - 0.577*my - 0.333*mz);
                frame.servos.calibrated[1] = C*( 0.667*mx            - 0.333*mz);
                frame.servos.calibrated[2] = C*(-0.333*mx + 0.577*my - 0.333*mz);
                frame.motor = ur;

                transmit(MSG_ID_MANUAL, QByteArray(reinterpret_cast<const char *>(&frame), sizeof(frame)));
            } else {
                msg_frame_setpoint_t frame;

                frame.rpy[0] = -30*DEG2RAD*gamepad.get(Gamepad::Analog::LX);
                frame.rpy[1] = +30*DEG2RAD*gamepad.get(Gamepad::Analog::LY);
                frame.rpy[2] = 0;
                frame.omega[0] = 0;
                frame.omega[1] = 0;
                frame.omega[2] = -90*DEG2RAD*gamepad.get(Gamepad::Analog::RX);
                frame.pos[0] = 0;
                frame.pos[1] = 0;
                frame.pos[2] = -0.5*gamepad.get(Gamepad::Analog::RY) + 0.5;
                frame.vel[0] = 0;
                frame.vel[1] = 0;
                frame.vel[2] = 0;

                transmit(MSG_ID_SETPOINT, QByteArray(reinterpret_cast<const char *>(&frame), sizeof(frame)));

                if(gamepad.get(Gamepad::Analog::VERTICAL)<0) {
                    transmit(MSG_ID_COMMAND_START, QByteArray());
                }

                if(gamepad.get(Gamepad::Button::X)) {
                    transmit(MSG_ID_COMMAND_ABORT, QByteArray());
                }

                if(gamepad.get(Gamepad::Button::B)) {
                    LiveChart::resume();
                }
            }
        });
        timer->start(20);

        connect(manual_switch, &QCheckBox::stateChanged, [timer, manual]() {
            manual[0]->setValue(0);
            manual[1]->setValue(0);
            manual[2]->setValue(0);
            manual[3]->setValue(0);
        });

        grid->addWidget(manual[0], 0, 0);
        grid->addWidget(manual[1], 0, 1);
        grid->addWidget(manual[2], 0, 2);
        grid->addWidget(manual[3], 0, 3);
        grid->addWidget(manual_switch, 1, 0, 3, 0);

        layout->addWidget(group, 1, 1, 2, 1);
    }

    {
        LiveChart::Config config;
        config.title = "Motor throttle";
        config.yLabel = "[%]";
        config.yMin = 0;
        config.yMax = 100;
        config.yPrecision = 0;
        config.yTick = 20;

        throttle = new LiveChart(config, this);
        throttle->addSeries("throttle", QPen(Qt::black, 2, Qt::SolidLine));

        layout->addWidget(throttle, 1, 2);
    }

    {
        LiveChart::Config config;
        config.title = "Attitude";
        config.yLabel = "[°]";
        config.yMin = -180;
        config.yMax = 180;
        config.yPrecision = 0;
        config.yTick = 45;

        attitude = new LiveChart(config, this);
        attitude->addSeries("roll setpoint",  QPen(Qt::red,   1, Qt::DashLine));
        attitude->addSeries("roll process",   QPen(Qt::red,   2, Qt::SolidLine));
        attitude->addSeries("pitch setpoint", QPen(Qt::green, 1, Qt::DashLine));
        attitude->addSeries("pitch process",  QPen(Qt::green, 2, Qt::SolidLine));
        attitude->addSeries("yaw process",    QPen(Qt::blue,  2, Qt::SolidLine));

        layout->addWidget(attitude, 1, 3);
    }

    {
        LiveChart::Config config;
        config.title = "Position";
        config.yLabel = "[m]";
        config.yMin = -2;
        config.yMax = 2;
        config.yPrecision = 0;
        config.yTick = 1;

        position = new LiveChart(config, this);
        position->addSeries("x setpoint", QPen(Qt::red,   1, Qt::DashLine));
        position->addSeries("x process",  QPen(Qt::red,   2, Qt::SolidLine));
        position->addSeries("y setpoint", QPen(Qt::green, 1, Qt::DashLine));
        position->addSeries("y process",  QPen(Qt::green, 2, Qt::SolidLine));
        position->addSeries("z setpoint", QPen(Qt::blue,  1, Qt::DashLine));
        position->addSeries("z process",  QPen(Qt::blue,  2, Qt::SolidLine));

        layout->addWidget(position, 1, 4);
    }

    {
        LiveChart::Config config;
        config.title = "Thrust vanes";
        config.yLabel = "[°]";
        config.yMin = -15;
        config.yMax = 15;
        config.yPrecision = 0;
        config.yTick = 5;

        fins = new LiveChart(config, this);
        fins->addSeries("vane 1", QPen(Qt::red,     2, Qt::SolidLine));
        fins->addSeries("vane 2", QPen(Qt::green,   2, Qt::SolidLine));
        fins->addSeries("vane 3", QPen(Qt::blue,    2, Qt::SolidLine));

        layout->addWidget(fins, 2, 2);
    }

    {
        LiveChart::Config config;
        config.title = "Angular velocity";
        config.yLabel = "[°/s]";
        config.yMin = -360;
        config.yMax = 360;
        config.yPrecision = 0;
        config.yTick = 90;

        angular_vel = new LiveChart(config, this);
        angular_vel->addSeries("x setpoint", QPen(Qt::red,   1, Qt::DashLine));
        angular_vel->addSeries("x process",  QPen(Qt::red,   2, Qt::SolidLine));
        angular_vel->addSeries("y setpoint", QPen(Qt::green, 1, Qt::DashLine));
        angular_vel->addSeries("y process",  QPen(Qt::green, 2, Qt::SolidLine));
        angular_vel->addSeries("z setpoint", QPen(Qt::blue,  1, Qt::DashLine));
        angular_vel->addSeries("z process",  QPen(Qt::blue,  2, Qt::SolidLine));

        layout->addWidget(angular_vel, 2, 3);
    }

    {
        LiveChart::Config config;
        config.title = "Velocity";
        config.yLabel = "[m/s]";
        config.yMin = -2;
        config.yMax = 2;
        config.yPrecision = 0;
        config.yTick = 1;

        linear_vel = new LiveChart(config, this);
        linear_vel->addSeries("x setpoint", QPen(Qt::red,   1, Qt::DashLine));
        linear_vel->addSeries("x process",  QPen(Qt::red,   2, Qt::SolidLine));
        linear_vel->addSeries("y setpoint", QPen(Qt::green, 1, Qt::DashLine));
        linear_vel->addSeries("y process",  QPen(Qt::green, 2, Qt::SolidLine));
        linear_vel->addSeries("z setpoint", QPen(Qt::blue,  1, Qt::DashLine));
        linear_vel->addSeries("z process",  QPen(Qt::blue,  2, Qt::SolidLine));

        layout->addWidget(linear_vel, 2, 4);
    }
}

void Window::receive(const uint8_t id, const double time, const QByteArray &payload) {
    if(id==MSG_ID_LOG) {
        if(settings.value("terminalSource").toInt()==1) {
            const int minutes = static_cast<int>(time) / 60;
            const double seconds = time - 60*minutes;

            std::cout << "[ ";
            std::cout << std::setfill('0') << std::setw(2) << minutes << ":";
            std::cout << std::setfill('0') << std::setw(6) << std::setprecision(3) << std::fixed << seconds;
            std::cout << " ] ";

            std::cout << std::string(reinterpret_cast<const char *>(payload.data()), payload.size());
            std::cout << std::endl;
        }

        return;
    }

    if(id==MSG_ID_PASSTHROUGH_GPS) {
        if(settings.value("terminalSource").toInt()==2) {
            std::cout << std::string(reinterpret_cast<const char *>(payload.data()), payload.size());
            std::cout.flush();
        }

        return;
    }

    if(id==MSG_ID_SENSOR && payload.size()==sizeof(msg_frame_sensor_t)) {
        const msg_frame_sensor_t *sensor = reinterpret_cast<const msg_frame_sensor_t *>(payload.data());

        if(settings.value("terminalSource").toInt()==3) {
            std::cout << *sensor << std::endl;
        }

        if(sensor->valid.barometer) {
            others->set("Pressure", "%.0f", sensor->barometer);
        }

        if(sensor->valid.rangefinder) {
            others->set("Distance", "%5.2f", sensor->rangefinder);
        }

        if(sensor->valid.tachometer) {
            others->set("Rotor velocity", "%.0f", sensor->tachometer);
        }

        if(sensor->valid.power) {
            others->set("Supply voltage", "%5.2f", sensor->power[0]);
            others->set("Supply current", "%5.2f", sensor->power[1]);
        }

        return;
    }

    if(id==MSG_ID_ESTIMATION && payload.size()==sizeof(msg_frame_estimation_t)) {
        const msg_frame_estimation_t *estimation = reinterpret_cast<const msg_frame_estimation_t *>(payload.data());

        if(settings.value("terminalSource").toInt()==4) {
            std::cout << *estimation << std::endl;
        }

        others->set("Magnetic inclination", "%+4.0f", estimation->theta_d*RAD2DEG);
        others->set("Ground pressure", "%.0f", estimation->pressure_0);

        return;
    }

    if(id==MSG_ID_CONTROLLER && payload.size()==sizeof(msg_frame_controller_t)) {
        const msg_frame_controller_t *controller = reinterpret_cast<const msg_frame_controller_t *>(payload.data());

        switch(controller->state) {
            case MSG_SM_STATE_READY: {
                others->set("State machine", "ready");
            } break;
            case MSG_SM_STATE_ACTIVE: {
                others->set("State machine", "active");
            } break;
            case MSG_SM_STATE_ABORT: {
                others->set("State machine", "abort");
            } break;
            case MSG_SM_STATE_MANUAL: {
                others->set("State machine", "manual");
            } break;
        }

        position->append("x process", time, controller->process.pos[0]);
        position->append("y process", time, controller->process.pos[1]);
        position->append("z process", time, controller->process.pos[2]);
        linear_vel->append("x process", time, controller->process.vel[0]);
        linear_vel->append("y process", time, controller->process.vel[1]);
        linear_vel->append("z process", time, controller->process.vel[2]);
        attitude->append("roll process", time, controller->process.rpy[0]*RAD2DEG);
        attitude->append("pitch process", time, controller->process.rpy[1]*RAD2DEG);
        attitude->append("yaw process", time, controller->process.rpy[2]*RAD2DEG);
        angular_vel->append("x process", time, controller->process.omega[0]*RAD2DEG);
        angular_vel->append("y process", time, controller->process.omega[1]*RAD2DEG);
        angular_vel->append("z process", time, controller->process.omega[2]*RAD2DEG);

        position->append("x setpoint", time, controller->setpoint.pos[0]);
        position->append("y setpoint", time, controller->setpoint.pos[1]);
        position->append("z setpoint", time, controller->setpoint.pos[2]);
        linear_vel->append("x setpoint", time, controller->setpoint.vel[0]);
        linear_vel->append("y setpoint", time, controller->setpoint.vel[1]);
        linear_vel->append("z setpoint", time, controller->setpoint.vel[2]);
        attitude->append("roll setpoint", time, controller->setpoint.rpy[0]*RAD2DEG);
        attitude->append("pitch setpoint", time, controller->setpoint.rpy[1]*RAD2DEG);
        //attitude->append("yaw setpoint", time, controller->setpoint.rpy[2]*RAD2DEG);
        angular_vel->append("x setpoint", time, controller->setpoint.omega[0]*RAD2DEG);
        angular_vel->append("y setpoint", time, controller->setpoint.omega[1]*RAD2DEG);
        angular_vel->append("z setpoint", time, controller->setpoint.omega[2]*RAD2DEG);

        throttle->append("throttle", time, controller->controls.throttle*100);
        fins->append("vane 1", time, controller->controls.angles[0]*RAD2DEG);
        fins->append("vane 2", time, controller->controls.angles[1]*RAD2DEG);
        fins->append("vane 3", time, controller->controls.angles[2]*RAD2DEG);

        LiveChart::synchronize(time);
    }
}
