#include <QApplication>
#include <QQmlApplicationEngine>

#include "usb.h"
#include "telnet.h"
#include "printer_writer.h"
#include "logger.h"
#include "battery.h"
#include "altimeter.h"
#include "scene.h"
#include "actuators.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	USB usb("/dev/ttyUSB0");
	Telnet telnet("192.168.64.29", 23);

	PrinterWriter printer_writer;
	QObject::connect(&usb, &USB::receive, &printer_writer, &PrinterWriter::onReceive);
	QObject::connect(&printer_writer, &PrinterWriter::onTransmit, &usb, &USB::transmit);
	QObject::connect(&telnet, &Telnet::receive, &printer_writer, &PrinterWriter::onReceive);
	QObject::connect(&printer_writer, &PrinterWriter::onTransmit, &telnet, &Telnet::transmit);

	Logger logger(engine);
	QObject::connect(&usb,    &USB::receive,    &logger, &Logger::receive);
	QObject::connect(&telnet, &Telnet::receive, &logger, &Logger::receive);

	Battery battery(engine);
	QObject::connect(&usb,    &USB::receive,    &battery, &Battery::receive);
	QObject::connect(&telnet, &Telnet::receive, &battery, &Battery::receive);

	Altimeter altimeter(engine);
	QObject::connect(&usb,    &USB::receive,    &altimeter, &Altimeter::receive);
	QObject::connect(&telnet, &Telnet::receive, &altimeter, &Altimeter::receive);

	Scene scene(engine);
	QObject::connect(&usb,    &USB::receive,    &scene, &Scene::receive);
	QObject::connect(&telnet, &Telnet::receive, &scene, &Scene::receive);

	Actuators actuators(engine);
	QObject::connect(&usb,    &USB::receive,    &actuators, &Actuators::receive);
	QObject::connect(&telnet, &Telnet::receive, &actuators, &Actuators::receive);

	return app.exec();
}
