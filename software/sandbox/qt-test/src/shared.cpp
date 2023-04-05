#include "shared.h"
#include <string>

Shared::Shared(QObject *parent) : QObject(parent) {
	
}

QString Shared::getData() {

	const int random_number = rand()%1000;

	data = QString::fromStdString("generated from C++ " + std::to_string(random_number));

	return data;
}
