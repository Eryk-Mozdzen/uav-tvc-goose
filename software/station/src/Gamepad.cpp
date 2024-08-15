#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <filesystem>
#include <thread>

#include <QTimer>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>

#include "Gamepad.h"

Gamepad::Gamepad(QWidget *parent) : QGroupBox{"Controller input", parent} {
    QGridLayout *layout = new QGridLayout(this);
    QFormLayout *form = new QFormLayout();

    addressComboBox = new QComboBox();
    addressComboBox->setMinimumWidth(125);
    connect(addressComboBox, &QComboBox::currentTextChanged, this, &Gamepad::changeInput);

    QPushButton *scanButton = new QPushButton("Scan inputs");
    connect(scanButton, &QPushButton::pressed, this, &Gamepad::scanInput);

    QPushButton *saveButton = new QPushButton("Save as default");
    connect(saveButton, &QPushButton::pressed, [this]() {
        settings.setValue("defaultInput", addressComboBox->currentText());
    });

    uiLabels[0] = new QLabel("---");
    uiLabels[1] = new QLabel("---");
    uiLabels[2] = new QLabel("---");
    uiLabels[3] = new QLabel("---");

    layout->setAlignment(Qt::AlignCenter);
    form->setLabelAlignment(Qt::AlignRight);
    form->addRow("LX / LY / LT:", uiLabels[0]);
    form->addRow("RX / RY / RT:", uiLabels[1]);
    form->addRow("DPAD:", uiLabels[2]);
    form->addRow("Buttons:", uiLabels[3]);

    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, [this]() {
        uiLabels[0]->setText(QString::asprintf("%+5.2f / %+5.2f / %+5.2f", get(Analog::LX), get(Analog::LY), get(Analog::LT)));
        uiLabels[1]->setText(QString::asprintf("%+5.2f / %+5.2f / %+5.2f", get(Analog::RX), get(Analog::RY), get(Analog::RT)));
        uiLabels[2]->setText(QString::asprintf("%+5.2f / %+5.2f", get(Analog::HORIZONTAL), get(Analog::VERTICAL)));

        QString text = "";
        text +=get(Button::X) ? "1" : "0";
        text +=get(Button::Y) ? "1" : "0";
        text +=get(Button::B) ? "1" : "0";
        text +=get(Button::A) ? "1" : "0";
        text +=get(Button::LB) ? "1" : "0";
        text +=get(Button::RB) ? "1" : "0";
        text +=get(Button::LSB) ? "1" : "0";
        text +=get(Button::RSB) ? "1" : "0";
        text +=get(Button::SELECT) ? "1" : "0";
        text +=get(Button::START) ? "1" : "0";
        text +=get(Button::HOME) ? "1" : "0";

        uiLabels[3]->setText(text);
    });
    timer->start(100);

    layout->addWidget(addressComboBox, 0, 0);
    layout->addWidget(scanButton, 1, 0);
    layout->addWidget(saveButton, 2, 0);
    layout->addLayout(form, 0, 1, 3, 2);

    setLayout(layout);

    scanInput();
}

Gamepad::~Gamepad() {
    thread_active = false;
    if(thread.joinable()) {
        thread.join();
    }
}

void Gamepad::process() {
    while(thread_active) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 1;

        if(select(fd + 1, &readfds, nullptr, nullptr, &timeout)>0) {
            struct js_event event;

            if(::read(fd, &event, sizeof(event))!=sizeof(event)) {
                ::close(fd);
                return;
            }

            switch(event.type) {
                case JS_EVENT_AXIS: {
                    analogs[event.number] = event.value/32768.;
                } break;
                case JS_EVENT_BUTTON: {
                    buttons[event.number] = event.value;
                } break;
            }
        }
    }
}

void Gamepad::scanInput() {
    addressComboBox->clear();

    for(const auto &entry : std::filesystem::directory_iterator("/dev/input/")) {
        const std::string path = entry.path().string();

        addressComboBox->addItem(path.c_str());
    }

    const int index = addressComboBox->findText(settings.value("defaultInput").toString());

    if(index != -1) {
        addressComboBox->setCurrentIndex(index);
    }

    changeInput(addressComboBox->currentText());
}

void Gamepad::changeInput(const QString &input) {
    thread_active = false;
    if(thread.joinable()) {
        thread.join();
    }

    fd = ::open(input.toStdString().c_str(), O_RDONLY);
    if(fd<0) {
        ::close(fd);
        return;
    }

    for(bool &val : buttons) {
        val = false;
    }
    for(double &val : analogs) {
        val = 0;
    }

    thread_active = true;
    thread = std::thread(&Gamepad::process, this);
}

double Gamepad::get(const Analog &analog) const {
    return analogs[analog];
}

bool Gamepad::get(const Button &button) const {
    return buttons[button];
}
