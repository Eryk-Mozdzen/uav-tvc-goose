#pragma once

#include <vector>
#include <QWidget>

#include "utils.h"

class Viewer : public QWidget {
	static constexpr int point = 5;
	static constexpr int size = 500;

	const std::vector<Sample> &samples;
	Params params;

	void paintEvent(QPaintEvent *event) override;

public:
    Viewer(const std::vector<Sample> &s, QWidget *parent=nullptr);

	void set(const Params &p);
};
