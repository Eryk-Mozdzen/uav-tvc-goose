#include "axis.h"
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>

Axis::Axis(const char name, QWidget *parent) : QWidget{parent} {

	QGridLayout *one = new QGridLayout(this);

	QGroupBox *group = new QGroupBox(QString(name) + " axis");

	one->addWidget(group, 0, 0);

	line_sensor = new QLineEdit();
	QPushButton *button_pos = new QPushButton("sample");
	QPushButton *button_neg = new QPushButton("sample");
	QLineEdit *line_pos = new QLineEdit();
	QLineEdit *line_neg = new QLineEdit();
	QLineEdit *line_offset = new QLineEdit();
	QLineEdit *line_scale = new QLineEdit();

	line_sensor->setReadOnly(true);
	line_sensor->setReadOnly(true);
	line_sensor->setPlaceholderText("max positive");
	line_sensor->setAlignment(Qt::AlignHCenter);
	line_pos->setReadOnly(true);
	line_pos->setPlaceholderText("max positive");
	line_pos->setAlignment(Qt::AlignHCenter);
	line_neg->setReadOnly(true);
	line_neg->setPlaceholderText("max negative");
	line_neg->setAlignment(Qt::AlignHCenter);
	line_offset->setReadOnly(true);
	line_offset->setPlaceholderText("offset");
	line_offset->setAlignment(Qt::AlignHCenter);
	line_scale->setReadOnly(true);
	line_scale->setPlaceholderText("scale");
	line_scale->setAlignment(Qt::AlignHCenter);

	QGridLayout *grid = new QGridLayout(group);
	grid->addWidget(line_sensor, 0, 0, 1, 2);
	grid->addWidget(button_pos, 1, 0);
	grid->addWidget(button_neg, 2, 0);
	grid->addWidget(line_pos, 1, 1);
	grid->addWidget(line_neg, 2, 1);
	grid->addWidget(line_offset, 3, 0, 1, 2);
	grid->addWidget(line_scale, 4, 0, 1, 2);

	connect(button_pos, &QPushButton::clicked, [this, line_pos, line_neg, line_offset, line_scale]() {
		const float pos = line_sensor->text().toFloat();

		line_pos->setText(QString::asprintf("%5.4f", pos));

		if(line_neg->text().size()>0) {
			const float neg = line_neg->text().toFloat();

			const float offset = 0.5f*(pos + neg);
			const float scale = g/(pos - offset);

			line_offset->setText(QString::asprintf("%5.4f", offset));
			line_scale->setText(QString::asprintf("%5.4f", scale));
		}
	});

	connect(button_neg, &QPushButton::clicked, [this, line_pos, line_neg, line_offset, line_scale]() {
		const float neg = line_sensor->text().toFloat();

		line_neg->setText(QString::asprintf("%5.4f", neg));

		if(line_pos->text().size()>0) {
			const float pos = line_pos->text().toFloat();

			const float offset = 0.5f*(pos + neg);
			const float scale = g/(pos - offset);

			line_offset->setText(QString::asprintf("%5.4f", offset));
			line_scale->setText(QString::asprintf("%5.4f", scale));
		}
	});

}

void Axis::set(const float sensor) {
    line_sensor->setText(QString::asprintf("%5.4f", sensor));
}
