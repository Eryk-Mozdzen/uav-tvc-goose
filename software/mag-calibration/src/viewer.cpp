#include "viewer.h"

Viewer::Viewer(const float s, QWidget *parent) : QWidget{parent}, scale{s} {
	setFixedSize(size, size);
	setBackgroundRole(QPalette::Window);
	setAutoFillBackground(true);
}

void Viewer::paintEvent(QPaintEvent *event) {
	(void)event;

	QPainter painter(this);
	painter.fillRect(rect(), Qt::black);

	constexpr float offset = size/2;

	for(const Sample &sample : samples) {
		const float x = scale*sample.x + offset;
		const float y = scale*sample.y + offset;
		const float z = scale*sample.z + offset;

		painter.setPen(Qt::red);
		painter.drawPoint(x, y);
		painter.setPen(Qt::green);
		painter.drawPoint(y, z);
		painter.setPen(Qt::blue);
		painter.drawPoint(z, x);
	}
}

void Viewer::add(const Sample &sample) {
	samples.append(sample);
	update();
}

void Viewer::clear() {
	samples.clear();
	update();
}
