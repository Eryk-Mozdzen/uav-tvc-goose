#include "gamepad.h"
#include <QDebug>

Gamepad::Gamepad(QObject *parent) : QObject{parent}, analogs{QVector<float>(6)}, buttons{QVector<bool>(15)} {

    QList<int> list = QGamepadManager::instance()->connectedGamepads();
    if (list.isEmpty()) {
        qDebug() << "Did not find any connected gamepads";
        return;
    }

	gamepad = new QGamepad(*list.begin(), this);

    connect(gamepad, &QGamepad::axisLeftXChanged,   this, std::bind(&Gamepad::updateAnalogs, this, Analog::LX, std::placeholders::_1));
    connect(gamepad, &QGamepad::axisLeftYChanged,   this, std::bind(&Gamepad::updateAnalogs, this, Analog::LY, std::placeholders::_1));
    connect(gamepad, &QGamepad::axisRightXChanged,  this, std::bind(&Gamepad::updateAnalogs, this, Analog::RX, std::placeholders::_1));
    connect(gamepad, &QGamepad::axisRightYChanged,  this, std::bind(&Gamepad::updateAnalogs, this, Analog::RY, std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonL2Changed,    this, std::bind(&Gamepad::updateAnalogs, this, Analog::L2, std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonR2Changed,    this, std::bind(&Gamepad::updateAnalogs, this, Analog::R2, std::placeholders::_1));

    connect(gamepad, &QGamepad::buttonAChanged,         this, std::bind(&Gamepad::updateButtons, this, Button::CIRCLE_A,    std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonBChanged,         this, std::bind(&Gamepad::updateButtons, this, Button::CIRCLE_B,    std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonXChanged,         this, std::bind(&Gamepad::updateButtons, this, Button::CIRCLE_X,    std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonYChanged,         this, std::bind(&Gamepad::updateButtons, this, Button::CIRCLE_Y,    std::placeholders::_1));

    connect(gamepad, &QGamepad::buttonDownChanged,      this, std::bind(&Gamepad::updateButtons, this, Button::CROSS_DOWN,  std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonUpChanged,        this, std::bind(&Gamepad::updateButtons, this, Button::CROSS_UP,    std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonLeftChanged,      this, std::bind(&Gamepad::updateButtons, this, Button::CROSS_LEFT,  std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonRightChanged,     this, std::bind(&Gamepad::updateButtons, this, Button::CROSS_RIGHT, std::placeholders::_1));

    connect(gamepad, &QGamepad::buttonL1Changed,        this, std::bind(&Gamepad::updateButtons, this, Button::L1,          std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonR1Changed,        this, std::bind(&Gamepad::updateButtons, this, Button::R1,          std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonL3Changed,        this, std::bind(&Gamepad::updateButtons, this, Button::L3,          std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonR3Changed,        this, std::bind(&Gamepad::updateButtons, this, Button::R3,          std::placeholders::_1));

    connect(gamepad, &QGamepad::buttonGuideChanged,     this, std::bind(&Gamepad::updateButtons, this, Button::HOME,        std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonSelectChanged,    this, std::bind(&Gamepad::updateButtons, this, Button::SELECT,      std::placeholders::_1));
    connect(gamepad, &QGamepad::buttonStartChanged,     this, std::bind(&Gamepad::updateButtons, this, Button::START,       std::placeholders::_1));
}

Gamepad::~Gamepad() {
    delete gamepad;
}

void Gamepad::updateAnalogs(Analog analog, float value) {
    analogs[analog] = value;
}

void Gamepad::updateButtons(Button button, bool value) {
    buttons[button] = value;
}

float Gamepad::get(Analog analog) const {
    return analogs[analog];
}

bool Gamepad::get(Button button) const {
    return buttons[button];
}
