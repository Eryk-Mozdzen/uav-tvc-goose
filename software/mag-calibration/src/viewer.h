#pragma once

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QVector>

class Viewer : public QWidget {
public:

	struct Sample {
		float x, y, z;
	};

    Viewer(const float s, QWidget *parent=nullptr);

	void add(const Sample &sample);
	void clear();

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	static constexpr int point = 4;
	static constexpr int size = 800;
	float scale;

	QVector<Sample> samples;
};
