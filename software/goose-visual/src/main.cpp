#include <QApplication>
#include <QQmlApplicationEngine>

#include "usb.h"
#include "printer_writer.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	USB usb("/dev/ttyUSB0");
	//USB usb("/dev/ttyACM0");

	PrinterWriter printer_writer;

	QObject::connect(&usb, &USB::receive, &printer_writer, &PrinterWriter::onReceive);
	QObject::connect(&printer_writer, &PrinterWriter::onTransmit, &usb, &USB::transmit);

	return app.exec();
}
