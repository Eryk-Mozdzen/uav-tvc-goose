#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "handler.h"
#include "shared.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	
	Shared shared;
	engine.rootContext()->setContextProperty("shared", &shared);
	
	QObject *item = engine.rootObjects().first();
    MyHandler handler;
    QObject::connect(item, SIGNAL(buttonClick(QString)), &handler, SLOT(click(QString)));
	QObject::connect(&handler, SIGNAL(doSomething()), item, SLOT(doSomething()));

    return app.exec();
}
