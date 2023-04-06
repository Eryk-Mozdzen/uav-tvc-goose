#include <QApplication>
#include <QQmlApplicationEngine>

#include "communication_usb.h"
#include "display.h"

int main(int argc, char *argv[]) {

	ConcurrentQueue<CommunicationBase::Log> logs;
	ConcurrentQueue<CommunicationBase::Telemetry> telemetry;
	ConcurrentQueue<CommunicationBase::Control> controls;

	CommunicationUSB usb("/dev/ttyACM0", logs, telemetry, controls);
	Display display(logs, telemetry);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
