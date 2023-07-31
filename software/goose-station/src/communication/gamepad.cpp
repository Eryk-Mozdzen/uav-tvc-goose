#include "gamepad.h"
#include <QGamepad>
#include <QDebug>
#include <QTimer>

Gamepad::Gamepad(QObject *parent) : QObject{parent}, analogs{QVector<float>(6)}, buttons{QVector<bool>(15)} {

    QTimer *timer = new QTimer(this);
    timer->setInterval(100);
    timer->start();

    connect(timer, &QTimer::timeout, [timer, this]() {
        const QList<int> list = QGamepadManager::instance()->connectedGamepads();

        if(list.isEmpty()) {
            return;
        }

        timer->stop();

        QGamepad *gamepad = new QGamepad(*list.begin(), this);

		qDebug() << "gamepad connected";

        connect(gamepad, &QGamepad::axisLeftXChanged,   	[this](float value) {analogs[Analog::LX] = value;});
        connect(gamepad, &QGamepad::axisLeftYChanged,   	[this](float value) {analogs[Analog::LY] = value;});
        connect(gamepad, &QGamepad::axisRightXChanged,  	[this](float value) {analogs[Analog::RX] = value;});
        connect(gamepad, &QGamepad::axisRightYChanged,  	[this](float value) {analogs[Analog::RY] = value;});
        connect(gamepad, &QGamepad::buttonL2Changed,    	[this](float value) {analogs[Analog::L2] = value;});
        connect(gamepad, &QGamepad::buttonR2Changed,    	[this](float value) {analogs[Analog::R2] = value;});

        connect(gamepad, &QGamepad::buttonAChanged,     	[this](bool value) {buttons[Button::CIRCLE_A] = value;});
        connect(gamepad, &QGamepad::buttonBChanged,     	[this](bool value) {buttons[Button::CIRCLE_B] = value;});
        connect(gamepad, &QGamepad::buttonXChanged,     	[this](bool value) {buttons[Button::CIRCLE_X] = value;});
        connect(gamepad, &QGamepad::buttonYChanged,     	[this](bool value) {buttons[Button::CIRCLE_Y] = value;});

        connect(gamepad, &QGamepad::buttonDownChanged,  	[this](bool value) {buttons[Button::CROSS_DOWN] = value;});
        connect(gamepad, &QGamepad::buttonUpChanged,    	[this](bool value) {buttons[Button::CROSS_UP] = value;});
        connect(gamepad, &QGamepad::buttonLeftChanged,  	[this](bool value) {buttons[Button::CROSS_LEFT] = value;});
        connect(gamepad, &QGamepad::buttonRightChanged, 	[this](bool value) {buttons[Button::CROSS_RIGHT] = value;});

        connect(gamepad, &QGamepad::buttonL1Changed,    	[this](bool value) {buttons[Button::L1] = value;});
        connect(gamepad, &QGamepad::buttonR1Changed,    	[this](bool value) {buttons[Button::R1] = value;});
        connect(gamepad, &QGamepad::buttonL3Changed,    	[this](bool value) {buttons[Button::L3] = value;});
        connect(gamepad, &QGamepad::buttonR3Changed,    	[this](bool value) {buttons[Button::R3] = value;});

        connect(gamepad, &QGamepad::buttonGuideChanged,		[this](bool value) {buttons[Button::HOME] = value;});
        connect(gamepad, &QGamepad::buttonSelectChanged,	[this](bool value) {buttons[Button::SELECT] = value;});
        connect(gamepad, &QGamepad::buttonStartChanged,		[this](bool value) {buttons[Button::START] = value;});

        connect(gamepad, &QGamepad::connectedChanged, [this, timer, gamepad](bool value) {
            if(value) {
                return;
            }

			for(float &value : analogs) {
				value = 0;
			}

			for(bool &value : buttons) {
				value = 0;
			}

            delete gamepad;

			qDebug() << "gamepad disconnected";

            timer->start();
        });
    });
}

float Gamepad::get(Analog analog) const {
    return analogs[analog];
}

bool Gamepad::get(Button button) const {
    return buttons[button];
}
