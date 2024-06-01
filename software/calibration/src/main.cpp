#include <QApplication>

#include "qt/Serial.h"
#include "qt/Telnet.h"
#include "Window.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	shared::Serial serial;
	shared::Telnet telnet;

	Window window;
	window.show();

	QObject::connect(&serial, &shared::Serial::receive, &window, &Window::receive);
	QObject::connect(&telnet, &shared::Telnet::receive, &window, &Window::receive);
	QObject::connect(&window, &Window::transmit, &serial, &shared::Serial::transmit);
	QObject::connect(&window, &Window::transmit, &telnet, &shared::Telnet::transmit);

	return app.exec();
}
