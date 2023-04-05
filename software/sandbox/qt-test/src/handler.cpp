#include "handler.h"

#include <iostream>

MyHandler::MyHandler(QObject *parent) : QObject(parent) {
	
}

void MyHandler::click(const QString &str) {
	std::cout << str.toStdString() << std::endl;

	doSomething();
}
