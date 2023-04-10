#include <QApplication>
#include <QQmlApplicationEngine>

#include "usb.h"
#include "telnet.h"
#include "printer_writer.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	USB usb("/dev/ttyACM0");
	Telnet telnet("192.168.64.29", 23);

	PrinterWriter printer_writer;

	QObject::connect(&usb, &USB::receive, &printer_writer, &PrinterWriter::onReceive);
	QObject::connect(&printer_writer, &PrinterWriter::onTransmit, &usb, &USB::transmit);

	QObject::connect(&telnet, &Telnet::receive, &printer_writer, &PrinterWriter::onReceive);
	QObject::connect(&printer_writer, &PrinterWriter::onTransmit, &telnet, &Telnet::transmit);

	return app.exec();
}
