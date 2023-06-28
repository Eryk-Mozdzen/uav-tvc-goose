#include "map.h"
#include <QWebEngineSettings>
#include <QHBoxLayout>

Map::Map(QWidget *parent) : QWidget{parent}, loaded{false} {
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    connect(view.page(), &QWebEnginePage::loadFinished, this, &Map::pageLoaded);

    view.page()->load(QUrl(QStringLiteral("qrc:///res/map.html")));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(&view);
    layout->setMargin(0);
}

void Map::pageLoaded() {
    loaded = true;
}

void Map::mark(double latitude, double longitude) {
    if(!loaded) {
        return;
    }

    const QString lat = QString::number(latitude, 'f', 20);
    const QString lng = QString::number(longitude, 'f', 20);
    const QString script = QString("addMarker(%1, %2);").arg(lat).arg(lng);

    view.page()->runJavaScript(script);
}
