#include <QApplication>
#include <QQmlApplicationEngine>

#include "communication_usb.h"
#include "display.h"
#include "writer.h"

int main(int argc, char *argv[]) {

	ConcurrentQueue<CommunicationBase::Log> logs;
	ConcurrentQueue<CommunicationBase::Telemetry> telemetry;
	ConcurrentQueue<CommunicationBase::Control> controls;

	CommunicationUSB usb(argv[1], logs, telemetry, controls);

	Display display(logs, telemetry);
	Writer writer(controls);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
