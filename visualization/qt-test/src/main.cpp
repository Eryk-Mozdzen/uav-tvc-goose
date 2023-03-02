#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "myclass.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
	
    QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	QObject *item = engine.rootObjects().first();
    MyClass myobject;
    QObject::connect(item, SIGNAL(menuClicked(QString)), &myobject, SLOT(onMenuClicked(QString)));

    return app.exec();
}
