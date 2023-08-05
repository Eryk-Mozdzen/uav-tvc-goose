#include "viewer.h"
#include <cmath>
#include <QPainter>

Viewer::Viewer(const QVector<Sample> &s, QWidget *parent) : QWidget{parent}, samples{s} {
	setFixedSize(size, size);
}

void Viewer::set(const Params &p) {
	params = p;
	update();
}

void Viewer::paintEvent(QPaintEvent *event) {
	(void)event;

	float max = -1.f;
	for(const Sample &sample : samples) {
		max = std::max(max, std::abs((sample.x - params.offset[0])/params.scale[0]));
		max = std::max(max, std::abs((sample.y - params.offset[1])/params.scale[1]));
		max = std::max(max, std::abs((sample.z - params.offset[2])/params.scale[2]));
	}
	const float window_scale = 0.4f*size/max;

	QPainter painter(this);
	painter.fillRect(rect(), Qt::black);
	painter.translate(rect().center());

	for(const Sample &sample : samples) {
		const float x = window_scale*(sample.x - params.offset[0])/params.scale[0];
		const float y = window_scale*(sample.y - params.offset[1])/params.scale[1];
		const float z = window_scale*(sample.z - params.offset[2])/params.scale[2];

		painter.setBrush(Qt::red);
		painter.drawEllipse(x, y, point, point);
		painter.setBrush(Qt::green);
		painter.drawEllipse(y, z, point, point);
		painter.setBrush(Qt::blue);
		painter.drawEllipse(z, x, point, point);
	}
}
