#include "myclass.h"

#include <iostream>

MyClass::MyClass(QObject *parent) : QObject(parent) {
	
}

void MyClass::onMenuClicked(const QString &str) {
	std::cout << str.toStdString() << std::endl;
}
