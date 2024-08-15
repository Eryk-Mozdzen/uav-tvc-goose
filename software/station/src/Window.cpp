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

#include "common/math/utils.h"
#include "common/protocol/msg.h"
#include "common/qt/Serial.h"
#include "common/qt/Network.h"
#include "Window.h"

std::ostream & operator<<(std::ostream &stream, const msg_frame_sensor_t sensor) {
	stream << "press";
	stream << std::setprecision(0) << std::fixed << std::noshowpos << std::setw(7);
	stream << (sensor.valid.barometer ? sensor.barometer : std::nan(""));

	stream << "   range";
	stream << std::setprecision(2) << std::fixed << std::noshowpos << std::setw(5);
	stream << (sensor.valid.rangefinder ? sensor.rangefinder : std::nan(""));

	stream << "   mag [";
	stream << std::setprecision(2) << std::fixed << std::showpos;
	stream << std::setw(6) << (sensor.valid.magnetometer ? sensor.magnetometer.calib[0] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.magnetometer ? sensor.magnetometer.calib[1] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.magnetometer ? sensor.magnetometer.calib[2] : std::nan(""));
	stream << "]";

	stream << "   accel [";
	stream << std::setprecision(2) << std::fixed << std::showpos;
	stream << std::setw(6) << (sensor.valid.accelerometer ? sensor.accelerometer.calib[0] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.accelerometer ? sensor.accelerometer.calib[1] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.accelerometer ? sensor.accelerometer.calib[2] : std::nan(""));
	stream << "]";

	stream << "   gyro [";
	stream << std::setprecision(2) << std::fixed << std::showpos;
	stream << std::setw(6) << (sensor.valid.gyroscope ? sensor.gyroscope.calib[0] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.gyroscope ? sensor.gyroscope.calib[1] : std::nan(""));
	stream << std::setw(6) << (sensor.valid.gyroscope ? sensor.gyroscope.calib[2] : std::nan(""));
	stream << "]";

	stream << "   gps [";
	stream << std::setprecision(6) << std::fixed << std::noshowpos;
	stream << std::setw(10) << (sensor.valid.gps ? sensor.gps[0] : std::nan(""));
	stream << std::setw(10) << (sensor.valid.gps ? sensor.gps[1] : std::nan(""));
	stream << "]";

	return stream;
}

std::ostream & operator<<(std::ostream &stream, const msg_frame_estimation_t estimation) {
	stream << "qua [";
	stream << std::setprecision(2) << std::fixed << std::showpos;
	stream << std::setw(6) << estimation.orientation[0];
	stream << std::setw(6) << estimation.orientation[1];
	stream << std::setw(6) << estimation.orientation[2];
	stream << std::setw(6) << estimation.orientation[3];
	stream << "]";

	stream << "   pos [";
	stream << std::setprecision(2) << std::fixed << std::showpos;
	stream << std::setw(6) << estimation.position[0];
	stream << std::setw(6) << estimation.position[1];
	stream << std::setw(6) << estimation.position[2];
	stream << "]";

	stream << "   vel [";
	stream << std::setprecision(2) << std::fixed << std::showpos;
	stream << std::setw(6) << estimation.velocity[0];
	stream << std::setw(6) << estimation.velocity[1];
	stream << std::setw(6) << estimation.velocity[2];
	stream << "]";

	return stream;
}

Window::Window(QWidget *parent) : QWidget(parent) {
    QGridLayout *layout = new QGridLayout(this);

    {
        common::Serial *serial = new common::Serial(this);
	    common::Network *network = new common::Network(this);

        connect(serial, &common::Serial::receive, this, &Window::receive);
        connect(network, &common::Network::receive, this, &Window::receive);
        connect(this, &Window::transmit, serial, &common::Serial::transmit);
        connect(this, &Window::transmit, network, &common::Network::transmit);

        layout->addWidget(serial, 0, 2);
        layout->addWidget(network, 0, 0, 1, 2);
    }

    {
        others = new Form("Others", {
            "state",
            "magnetic inclination",
            "ground pressure",
            "pressure",
            "distance",
            "rotor velocity",
            "supply voltage",
            "supply current"
        });

        layout->addWidget(others, 1, 0);
    }

    {
        QGroupBox *group = new QGroupBox("Controls", this);
        QVBoxLayout *inner = new QVBoxLayout(group);

        QPushButton *cmd_start = new QPushButton("Start", this);
        QPushButton *cmd_abort = new QPushButton("Abort", this);
        QPushButton *resume = new QPushButton("Resume", this);
        QPushButton *save = new QPushButton("Save", this);
        QRadioButton *source1 = new QRadioButton("Logger", this);
        QRadioButton *source2 = new QRadioButton("GPS passthrough", this);
        QRadioButton *source3 = new QRadioButton("Sensor readings", this);
        QRadioButton *source4 = new QRadioButton("Estimation", this);

        inner->addWidget(cmd_start);
        inner->addWidget(cmd_abort);
        inner->addWidget(resume);
        inner->addWidget(save);
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

        /*QTimer *timer = new QTimer();
        connect(timer, &QTimer::timeout, [this]() {
            comm::Controller::State setpoint;

            setpoint.rpy[0] = -30.f*deg2rad*gamepad.get(Gamepad::Analog::LX);
            setpoint.rpy[1] = +30.f*deg2rad*gamepad.get(Gamepad::Analog::LY);
            setpoint.rpy[2] = 0.f;
            setpoint.w[0] = 0.f;
            setpoint.w[1] = 0.f;
            setpoint.w[2] = -90.f*deg2rad*gamepad.get(Gamepad::Analog::RX);
            setpoint.z = -0.5f*gamepad.get(Gamepad::Analog::RY) + 0.5f;
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

            if(gamepad.get(Gamepad::Button::CIRCLE_B)) {
                widgets::LiveChart::resume();
            }
        });
        timer->start(20);*/
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
        timer->setInterval(20);
        connect(timer, &QTimer::timeout, [this, manual]() {
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
        });

        connect(manual_switch, &QCheckBox::stateChanged, [timer, manual](int state) {
            manual[0]->setValue(0);
            manual[1]->setValue(0);
            manual[2]->setValue(0);
            manual[3]->setValue(0);

            if(state) {
                timer->start();
            } else {
                timer->stop();
            }
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
        config.title = "Motor Throttle";
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
        attitude->addSeries("roll setpoint", QPen(Qt::red,      1, Qt::DashLine));
        attitude->addSeries("roll process", QPen(Qt::red,       2, Qt::SolidLine));
        attitude->addSeries("pitch setpoint", QPen(Qt::green,   1, Qt::DashLine));
        attitude->addSeries("pitch process", QPen(Qt::green,    2, Qt::SolidLine));
        attitude->addSeries("yaw process", QPen(Qt::blue,       2, Qt::SolidLine));

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
        position->addSeries("x setpoint", QPen(Qt::red,    1, Qt::DashLine));
        position->addSeries("x process", QPen(Qt::red,     2, Qt::SolidLine));
        position->addSeries("y setpoint", QPen(Qt::green,  1, Qt::DashLine));
        position->addSeries("y process", QPen(Qt::green,   2, Qt::SolidLine));
        position->addSeries("z setpoint", QPen(Qt::blue,   1, Qt::DashLine));
        position->addSeries("z process", QPen(Qt::blue,    2, Qt::SolidLine));

        layout->addWidget(position, 1, 4);
    }

    {
        LiveChart::Config config;
        config.title = "Thrust Vanes";
        config.yLabel = "[°]";
        config.yMin = -15;
        config.yMax = 15;
        config.yPrecision = 0;
        config.yTick = 5;

        fins = new LiveChart(config, this);
        fins->addSeries("vane 1", QPen(Qt::red,      2, Qt::SolidLine));
        fins->addSeries("vane 2", QPen(Qt::green,    2, Qt::SolidLine));
        fins->addSeries("vane 3", QPen(Qt::blue,     2, Qt::SolidLine));
        fins->addSeries("vane 4", QPen(Qt::magenta,  2, Qt::SolidLine));

        layout->addWidget(fins, 2, 2);
    }

    {
        LiveChart::Config config;
        config.title = "Angular Velocity";
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

void Window::receive(const uint8_t id, const QByteArray &payload) {
    if(id==MSG_ID_LOG) {
        if(settings.value("terminalSource").toInt()==1) {
            std::cout << std::string(reinterpret_cast<const char *>(payload.data()), payload.size()) << std::endl;
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

        return;
    }

    if(id==MSG_ID_ESTIMATION && payload.size()==sizeof(msg_frame_estimation_t)) {
        const msg_frame_estimation_t *estimation = reinterpret_cast<const msg_frame_estimation_t *>(payload.data());

        if(settings.value("terminalSource").toInt()==4) {
            std::cout << *estimation << std::endl;
        }

        float rpy[3];
        utils_quaternion_to_rpy(estimation->orientation, rpy);

        others->set("magnetic inclination", "%+6.0f", estimation->theta_d*RAD2DEG);
        others->set("ground pressure", "%6.0f", estimation->pressure_0);
        position->append("x process", estimation->position[0]);
        position->append("y process", estimation->position[1]);
        position->append("z process", estimation->position[2]);
        linear_vel->append("x process", estimation->velocity[0]);
        linear_vel->append("y process", estimation->velocity[1]);
        linear_vel->append("z process", estimation->velocity[2]);
        attitude->append("roll process", rpy[0]*RAD2DEG);
        attitude->append("pitch process", rpy[1]*RAD2DEG);
        attitude->append("yaw process", rpy[2]*RAD2DEG);
        angular_vel->append("x process", estimation->angular_velocity[0]*RAD2DEG);
        angular_vel->append("y process", estimation->angular_velocity[1]*RAD2DEG);
        angular_vel->append("z process", estimation->angular_velocity[2]*RAD2DEG);

        return;
    }
}
