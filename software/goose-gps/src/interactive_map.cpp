#include "interactive_map.h"
#include <QWebEngineSettings>
#include <QHBoxLayout>

InteractiveMap::InteractiveMap(QWidget *parent) : QWidget{parent}, view{new QWebEngineView(this)}, loaded{false} {
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    connect(view->page(), &QWebEnginePage::loadFinished, this, &InteractiveMap::pageLoaded);

    view->page()->load(QUrl(QStringLiteral("qrc:/file.html")));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(view);
    layout->setMargin(0);

}

void InteractiveMap::pageLoaded() {
    loaded = true;
}

void InteractiveMap::mark(double latitude, double longitude) {
    if(loaded) {
        const QString script = QString("addMarker(%1, %2);").arg(QString::number(latitude, 'f', 20)).arg(QString::number(longitude, 'f', 20));

        view->page()->runJavaScript(script);
    }
}
