#include <cmath>
#include <QPainter>

#include "Viewer.h"

Viewer::Viewer(const std::vector<Sample> &s, QWidget *parent) : QWidget{parent}, samples{s} {
	setFixedSize(size, size);
}

void Viewer::set(const Params &p) {
	params = p;
}

void Viewer::paintEvent(QPaintEvent *event) {
	(void)event;

	float max = -1.f;
	for(const Sample &sample : samples) {
		const Sample conv = params*sample;

		max = std::max(max, std::abs((conv.x)));
		max = std::max(max, std::abs((conv.y)));
		max = std::max(max, std::abs((conv.z)));
	}
	const float window_scale = 0.4f*size/max;

	QPainter painter(this);
	painter.fillRect(rect(), Qt::transparent);
	painter.translate(rect().center());
	painter.setPen(QPen(Qt::transparent));

	for(const Sample &sample : samples) {
		const Sample conv = params*sample;

		const float x = window_scale*conv.x;
		const float y = window_scale*conv.y;
		const float z = window_scale*conv.z;

		painter.setBrush(Qt::red);
		painter.drawEllipse(x, y, point, point);
		painter.setBrush(Qt::darkGreen);
		painter.drawEllipse(y, z, point, point);
		painter.setBrush(Qt::blue);
		painter.drawEllipse(z, x, point, point);
	}
}
