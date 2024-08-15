#pragma once

#include <thread>

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>

class Gamepad : public QGroupBox {
    Q_OBJECT

    std::thread thread;
    bool thread_active;

    QComboBox *addressComboBox;
    QSettings settings;
	QLabel *uiLabels[4];

    int fd;
    fd_set readfds;
    struct timeval timeout;
    double analogs[20];
    bool buttons[20];

    void process();

private slots:
    void scanInput();
	void changeInput(const QString &input);

public:
    enum Analog {
        LX = 0,
        LY = 1,
        LT = 2,
        RX = 3,
        RY = 4,
        RT = 5,
        HORIZONTAL = 6,
        VERTICAL = 7,
    };

    enum Button {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        LB = 4,
        RB = 5,
        SELECT = 6,
        START = 7,
        HOME = 8,
        LSB = 9,
        RSB = 10,
    };

    Gamepad(QWidget *parent = nullptr);
    ~Gamepad();
    double get(const Analog &analog) const;
    bool get(const Button &button) const;
};
