#pragma once

#include <QWidget>
#include <QVector>
#include "utils.h"

class Viewer : public QWidget {
	static constexpr int point = 4;
	static constexpr int size = 500;

	const QVector<Sample> &samples;
	Params params;

	void paintEvent(QPaintEvent *event) override;

public:
    Viewer(const QVector<Sample> &s, QWidget *parent=nullptr);

	void set(const Params &p);
};
