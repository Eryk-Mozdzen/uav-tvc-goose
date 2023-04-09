#include <QApplication>
#include <QQmlApplicationEngine>

#include "usb.h"
#include "telnet.h"
#include "printer.h"
#include "writer.h"

int main(int argc, char *argv[]) {

	Comm commuication;

	//USB uart("/dev/ttyUSB0", commuication);
	USB usb("/dev/ttyACM0", commuication);
	Telnet telnet("192.168.113.29", "23", commuication);

	Printer printer(commuication);
	Writer writer(commuication);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
