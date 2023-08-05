#pragma once

#include <QLineEdit>

class Axis : public QWidget {
	Q_OBJECT

	static constexpr float g = 9.81f;

	QLineEdit *line_sensor;

public:
    Axis(const char name, QWidget *parent=nullptr);

    void set(const float sensor);
};
