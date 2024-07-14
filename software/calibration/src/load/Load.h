#pragma once

#include <QLineEdit>

#include "Interface.h"

class Load : public Interface {
    static constexpr int num = 100;
    int data[num] = {0};
    int sum = 0;
    int counter = 0;

    int min = 8500000;
    int max = 9500000;

    QLineEdit *load_line;
    QLineEdit *raw_line;
    QLineEdit *avg_line;
    QLineEdit *min_line;
    QLineEdit *max_line;

public:
    Load(QWidget *parent = nullptr);

    Interface * create() const;
    void receive(const protocol_readings_t &readings);
    void update(protocol_calibration_t &calibration) const;
};
