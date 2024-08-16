#include <QTimer>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QMetaType>

#include "Gamepad.h"
#include "Joystick.h"

Q_DECLARE_METATYPE(utils::Joystick::Info)

Gamepad::Gamepad(QWidget *parent) : QGroupBox{"Controller input", parent} {
    QGridLayout *layout = new QGridLayout(this);
    QFormLayout *form = new QFormLayout();

    addressComboBox = new QComboBox();
    addressComboBox->setMinimumWidth(200);
    connect(addressComboBox, &QComboBox::currentIndexChanged, this, &Gamepad::changeInput);

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

    uiLabels[0]->setMinimumWidth(150);

    layout->setAlignment(Qt::AlignCenter);
    form->setLabelAlignment(Qt::AlignRight);
    form->addRow("LX / LY / LT:", uiLabels[0]);
    form->addRow("RX / RY / RT:", uiLabels[1]);
    form->addRow("DPAD:", uiLabels[2]);
    form->addRow("Buttons:", uiLabels[3]);

    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, [this]() {
        if(!joystick) {
            return;
        }

        if(!joystick->isAvailable()) {
            delete joystick;
            joystick = nullptr;
            addressComboBox->clear();
            return;
        }

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
    if(joystick) {
        delete joystick;
    }
}

void Gamepad::scanInput() {
    addressComboBox->clear();

    const auto available = utils::Joystick::getAvailable();

	for(const auto &info : available) {
        addressComboBox->addItem(info.name.c_str(), QVariant::fromValue(info));
	}

    const int index = addressComboBox->findText(settings.value("defaultInput").toString());

    if(index != -1) {
        addressComboBox->setCurrentIndex(index);
    }

    changeInput(addressComboBox->currentIndex());
}

void Gamepad::changeInput(const int index) {
    if(addressComboBox->count()==0) {
        return;
    }

    if(joystick) {
        delete joystick;
        joystick = nullptr;
    }

    const utils::Joystick::Info info = addressComboBox->itemData(index).value<utils::Joystick::Info>();



    joystick = new utils::Joystick(info);
}

double Gamepad::get(const Analog &analog) const {
    return joystick ? joystick->getAxis(analog) : 0;
}

bool Gamepad::get(const Button &button) const {
    return joystick ? joystick->getButton(button) : false;
}
